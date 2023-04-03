#include <algorithm>
#include <deque>
#include <sstream>
#include <tuple>
#include <unordered_set>

#include "engine/engine.hpp"

#include "path-finder.hpp"
#include "sprite-info.hpp"
#include "world.hpp"

class OpenListEntry {
public:
    OpenListEntry(int tile, int cost) : tile(tile), cost(cost) {}

    bool operator==(int other) const {
        return tile == other;
    }

    int tile, cost;
};

class PathFinderInternalState {
public:
    std::vector<int> tile_parents, tile_costs;
    std::unordered_set<int> closed, open;
    std::deque<OpenListEntry> open_order;
};

PathFinder::Path::Path() {
}

PathFinder::State::State(blit::Size size, int max_dist) : entity_size(size), max_dist(max_dist), done(false), internal(std::make_unique<PathFinderInternalState>()) {
}

PathFinder::State::~State() {
}

PathFinder::PathFinder(World &world) : world(world), next_path_key(1) {

}

PathFinder::~PathFinder() {
}

bool PathFinder::find_path(const blit::Point &start_pos, const blit::Point &end_pos, State &state, Path &path) {
    auto bounds = world.get_bounds();

    const int width = bounds.w;
    const int height = bounds.h;

    const int start_tile = start_pos.x + start_pos.y * width;
    const int end_tile = end_pos.x + end_pos.y * width;

    if(start_tile < 0 || end_tile < 0 || start_pos.x >= width || start_pos.y >= height || end_pos.x >= width || end_pos.y >= height || start_tile == end_tile) {
        path.path.clear();
        return true;
    }

    // can't enter end tile
    if(state.max_dist == 0 && get_tile_collision(state, end_pos.x, end_pos.y) == CD_All) {
        path.path.clear();
        return true;
    }

    // distance
    auto get_h = [&] (int tX, int tY) {return (abs(tX - end_pos.x) + abs(tY - end_pos.y)) * 10;};

    // setup state
    auto &internal_state = *state.internal;
    if(state.start_pos != start_pos || state.end_pos != end_pos || internal_state.tile_costs.empty()) {
        internal_state.tile_costs.clear();
        internal_state.tile_parents.clear();
        internal_state.open.clear();
        internal_state.closed.clear();
        internal_state.open_order.clear();

        internal_state.tile_parents.resize(width * height);
        internal_state.tile_costs.resize(width * height);
        internal_state.open_order.push_back(OpenListEntry(start_tile, get_h(start_pos.x, start_pos.y)));
        internal_state.open.insert(start_tile);
        internal_state.tile_costs[start_tile] = 0;

        state.start_pos = start_pos;
        state.end_pos = end_pos;
    }

    int tile_size = 10; //

    int iters = 0;
    const int max_iters = 200;

    auto start_time = blit::now();

    while(!internal_state.open_order.empty()) {
        if(iters == max_iters) {
            auto time_taken = blit::now() - start_time;

            if(time_taken > 5) {
                blit::debugf("Path find from %i, %i to %i, %i paused after taking %u ms (%i iterations)\n", start_pos.x, start_pos.y,
                             end_pos.x, end_pos.y, time_taken, iters);
            }
            return false;
        }

        int tile = internal_state.open_order.front().tile;

        //found it, or in range
        if(tile == end_tile || (state.max_dist && get_h(tile % width, tile / width) / 10 <= state.max_dist)) {
            path.path.clear();
            //path.path.push_back(start_pos);

            int cur_tile = tile;

            while(cur_tile != start_tile) {
                int curX = cur_tile % width;
                int curY = cur_tile / width;
                path.path.emplace(path.path.begin(), curX, curY);

                cur_tile = internal_state.tile_parents[cur_tile];
            }

            path.path.insert(path.path.begin(), start_pos);


            auto time_taken = blit::now() - start_time;
            if(time_taken > 5) {
                blit::debugf("Path find from %i, %i to %i, %i succeeded taking %u ms (%i iterations)\n", start_pos.x, start_pos.y, end_pos.x,
                             end_pos.y, time_taken, iters);
            }
            return true;
        }

        internal_state.open_order.pop_front();
        internal_state.open.erase(tile);
        internal_state.closed.insert(tile);

        auto neighbours = get_neighbours(state, tile);

        for(int neighbour : neighbours) {
            if(internal_state.closed.count(neighbour) != 0)
                continue;

            bool in_open_list = internal_state.open.count(neighbour) != 0;
            //new tile
            if(!in_open_list) {
                internal_state.tile_parents[neighbour] = -1;
                internal_state.tile_costs[neighbour] = -1;
            }

            //update
            int old_cost = internal_state.tile_costs[neighbour];
            //compute cost

            int this_cost = tile_size;

            int x = neighbour % width;
            int y = neighbour / width;


            int prev_x = tile % width;
            int prev_y = tile / width;
            //diagonal
            if(abs(x - prev_x) && abs(y - prev_y))
                this_cost = static_cast<int>(sqrt(this_cost * this_cost * 2));

            int g_use_this = internal_state.tile_costs[tile] + this_cost;

            if(g_use_this < old_cost || old_cost == -1) {
                internal_state.tile_parents[neighbour] = tile;
                internal_state.tile_costs[neighbour] = g_use_this;
            }

            //end compute cost

            //cost decreased
            if(internal_state.tile_costs[neighbour] < old_cost || old_cost == -1) {
                auto &open_order = internal_state.open_order;

                //remove and reinsert with new cost
                int h = get_h(x, y);

                int new_cost = internal_state.tile_costs[neighbour] + h;

                //find higher/equal cost
                auto comp = [](const OpenListEntry &a, int b){return a.cost < b;};
                auto not_less_it = std::lower_bound(open_order.begin(), open_order.end(), new_cost, comp);

                //insert before to keep list sorted
                auto new_it = open_order.insert(not_less_it, OpenListEntry(neighbour, new_cost));

                if(in_open_list) {
                    //cost decreased, old pos will be later than new pos
                    auto this_it = std::find(new_it + 1, open_order.end(), neighbour);
                    open_order.erase(this_it);
                }
                else
                    internal_state.open.insert(neighbour);
            }
        }
        iters++;
    }

    auto time_taken = blit::now() - start_time;
    if(time_taken > 5) {
        blit::debugf("Path find from %i, %i to %i, %i failed taking %u ms\n", start_pos.x, start_pos.y, end_pos.x, end_pos.y,
                     time_taken);
    }

    path.path.clear();
    return true;
}

unsigned int PathFinder::start_path_find(const blit::Point &start_pos, const blit::Point &end_pos, blit::Size size, int max_dist) {
    State state(size, max_dist);
    state.start_pos = start_pos;
    state.end_pos = end_pos;
    paths.emplace(next_path_key, std::move(state));
    return next_path_key++;
}

bool PathFinder::is_path_find_done(unsigned int key) const {
    auto it = paths.find(key);
    if(it == paths.end())
        return false;

    return it->second.done;
}

void PathFinder::get_found_path(unsigned int key, Path &path) const {
    path.path.clear();

    auto it = paths.find(key);
    if(it == paths.end())
        return;

    auto &internal_state = *it->second.internal;

    // failed
    if(internal_state.open_order.empty())
        return;

    const int width = world.get_bounds().w;
    const int start_tile = it->second.start_pos.x + it->second.start_pos.y * width;

    int cur_tile = internal_state.open_order.front().tile;

    while(cur_tile != start_tile) {
        int curX = cur_tile % width;
        int curY = cur_tile / width;
        path.path.emplace(path.path.begin(), curX, curY);

        cur_tile = internal_state.tile_parents[cur_tile];
    }

    path.path.insert(path.path.begin(), it->second.start_pos);
}

void PathFinder::remove_path_find(unsigned int key) {
    paths.erase(key);
}

void PathFinder::update() {
    Path tmp;
    for(auto &path : paths) {
        if(path.second.done)
            continue;

        path.second.done = find_path(path.second.start_pos, path.second.end_pos, path.second, tmp);
    }
}

int PathFinder::get_tile_collision(State &state, unsigned int x, unsigned int y, bool check_ents) const {
    // handle all tiles this enity will cover as one large tile
    int ret = 0;

    for(int y_off = 0; y_off < state.entity_size.h; y_off++) {
        for(int x_off = 0; x_off < state.entity_size.w; x_off++) {
            ret |= get_single_tile_collision(state, x + x_off, y + y_off, check_ents);
        }
    }

    return ret;
}

int PathFinder::get_single_tile_collision(State &state, unsigned int x, unsigned int y, bool check_ents) const {
    auto bounds = world.get_bounds();

    // handle large units at the map edge
    if(x >= static_cast<unsigned int>(bounds.w) || y >= static_cast<unsigned int>(bounds.h))
        return CD_Up | CD_Down | CD_Left | CD_Right;

    int ret = 0;

    blit::Point pos(static_cast<int>(x), static_cast<int>(y));
    auto tile = world.get_tile(pos.x, pos.y);

    const auto wall_passable = [](int wall){
        // TODO: less don't hardcode doors
        return wall == 0 || wall == 3/*door*/;
    };

    // check for walls
    if(!wall_passable(tile->walls[Side_Bottom]))
        ret |= CD_Down;
    if(!wall_passable(tile->walls[Side_Right]))
        ret |= CD_Right;
    if(!wall_passable(tile->walls[Side_Top]))
        ret |= CD_Up;
    if(!wall_passable(tile->walls[Side_Left]))
        ret |= CD_Left;

    if(!check_ents)
        return ret;

    // ignore tiles the entity we're moving is on
    if(pos.x >= state.start_pos.x && pos.x < state.start_pos.x + state.entity_size.w && pos.y >= state.start_pos.y && pos.y < state.start_pos.y + state.entity_size.h)
        return ret;

    // ignore entities on the destination as well
    if(pos.x >= state.end_pos.x && pos.x < state.end_pos.x + state.entity_size.w && pos.y >= state.end_pos.y && pos.y < state.end_pos.y + state.entity_size.h)
        return ret;

    // check for entities
    for(auto ent : tile->entities) {
        if(ent) // TODO: allow moving through some things?
            return CD_Up | CD_Down | CD_Left | CD_Right;
    }


    return ret;
}

bool PathFinder::can_move_to(int src_collision, int dst_collision, int xDiff, int yDiff) const {
    if(src_collision == 0 && dst_collision == 0)
        return true;

    if(yDiff > 0 && !(dst_collision & CD_Up) && !(src_collision & CD_Down))
        return true;

    if(yDiff < 0 && !(dst_collision & CD_Down) && !(src_collision & CD_Up))
        return true;

    if(xDiff > 0 && !(dst_collision & CD_Left) && !(src_collision & CD_Right))
        return true;

    if(xDiff < 0 && !(dst_collision & CD_Right) && !(src_collision & CD_Left))
        return true;

    return false;
}

std::vector<int> PathFinder::get_neighbours(State &state, int tile) const {
    auto bounds = world.get_bounds();

    const int width = bounds.w;
    const int height = bounds.h;

    const int x = tile % width;
    const int y = tile / width;

    const std::tuple<int, int> possible_neighbours[] { {x - 1, y    }, {x + 1, y    }, {x    , y - 1}, {x    , y + 1},

        //diag {x - 1, y - 1}, {x - 1, y + 1}, {x + 1, y - 1}, {x + 1, y + 1}
    };

    std::vector<int> neighbours;
    neighbours.reserve(8);

    for(const auto &pos : possible_neighbours) {
        int newX = std::get<0>(pos), newY = std::get<1>(pos);

        if(newX < state.entity_size.w - 1 || newY < state.entity_size.h - 1 || newX > width - state.entity_size.w || newY > height - state.entity_size.h)
            continue;

        if(abs(x - newX) && abs(y - newY)) {
            if(get_tile_collision(state, x, newY) != 0 || get_tile_collision(state, newX, y) != 0)
                continue; //cutting corner
        }

        auto src_collision = get_tile_collision(state, x, y, false);
        auto dst_collision = get_tile_collision(state, newX, newY);

        bool can_move = can_move_to(src_collision, dst_collision, newX - x, newY - y);

        if(can_move)
            neighbours.push_back(newX + newY * width);
    }

    return neighbours;
}