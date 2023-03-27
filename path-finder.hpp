#pragma once

#include <map>
#include <memory>
#include <vector>

#include "types/point.hpp"
#include "types/size.hpp"

class World;

class PathFinderInternalState;

class PathFinder {
public:
    class Path {
    public:
        Path();

        std::vector<blit::Point> path;
    };

    class State {
    public:
        State(blit::Size size = {1, 1});
        State(State &&) = default;
        ~State();

        State &operator=(State &&) = default;

        blit::Size entity_size; // how many tiles the thing we're finding a path for takes up

        blit::Point start_pos, end_pos;
        bool done;
        std::unique_ptr<PathFinderInternalState> internal;
    };

    PathFinder(World &world);
    virtual ~PathFinder();

    bool find_path(const blit::Point &start_pos, const blit::Point &end_pos, State &state, Path &path);

    unsigned int start_path_find(const blit::Point &start_pos, const blit::Point &end_pos, blit::Size size);
    bool is_path_find_done(unsigned int key) const;
    void get_found_path(unsigned int key, Path &path) const;
    void remove_path_find(unsigned int key);

    void update();

protected:
    enum CollisionDirection {
        CD_Up    = 1,
        CD_Down  = 2,
        CD_Left  = 4,
        CD_Right = 8,

        CD_All = CD_Up | CD_Down | CD_Left | CD_Right
    };

    World &world;

    int get_tile_collision(State &state, unsigned int x, unsigned int y) const;
    int get_single_tile_collision(State &state, unsigned int x, unsigned int y) const;
    bool can_move_to(int collision, int xDiff, int yDiff) const;
    std::vector<int> get_neighbours(State &state, int tile) const;

    std::map<unsigned int, State> paths;
    unsigned int next_path_key;
};