#include <cstring>
#include <utility>

#include "engine/engine.hpp"

#include "world.hpp"

#include "assets.hpp"
#include "sprite-info.hpp"

// TODO: this is getting silly, need something to edit this with
static const blit::Pen default_col{}, yellow_col{255, 255, 128}, cyan_col{200, 255, 255}, green_col{50, 100, 50},
                       off_white_col{240, 240, 240}, grey60_col{60, 60, 60}, grey100_col{100, 100, 100};

static const MapTile default_chunk[]{
    // Y = 0
    {1, {0, 0, 1, 1},  {},  grey60_col, {default_col, default_col, off_white_col, off_white_col}},
    {1, {0, 0, 1, 0},  {}, grey100_col, {default_col, default_col, off_white_col,   default_col}},
    {1, {0, 0, 2, 0},  {},  grey60_col, {default_col, default_col, off_white_col,   default_col}},
    {1, {0, 1, 1, 0},  {}, grey100_col, {default_col,  yellow_col, off_white_col,   default_col}},
    {1, {0, 0, 1, 0},  {}, default_col, {default_col, default_col,    yellow_col,   default_col}},
    {1, {0, 0, 1, 0},  {}, default_col, {default_col, default_col,    yellow_col,   default_col}},
    {1, {0, 0, 1, 0},  {}, default_col, {default_col, default_col,    yellow_col,   default_col}},
    {1, {0, 0, 2, 0},  {}, default_col, {default_col, default_col,    yellow_col,   default_col}},
    {1, {0, 0, 1, 0},  {}, default_col, {default_col, default_col,    yellow_col,   default_col}},
    {1, {0, 1, 1, 0},  {}, default_col, {default_col, default_col,    yellow_col,   default_col}},
    {},
    {},
    {},
    {},
    {},
    {},
    // Y = 1
    {1, {0, 0, 0, 1},  {}, grey100_col, {default_col, default_col,   default_col, off_white_col}},
    {1, {0, 0, 0, 0},  {},  grey60_col},
    {1, {0, 0, 0, 0},  {}, grey100_col},
    {1, {0, 1, 0, 0},  {},  grey60_col, {default_col,  yellow_col,   default_col,   default_col}},
    {1, {0, 0, 0, 0}},
    {1, {0, 0, 0, 0}},
    {1, {0, 0, 0, 0}},
    {1, {0, 0, 0, 0}},
    {1, {0, 0, 0, 0}},
    {1, {0, 1, 0, 0}},
    {},
    {},
    {},
    {},
    {},
    {},
    // Y = 2
    {1, {0, 0, 0, 1},  {},  grey60_col, {default_col, default_col,   default_col, off_white_col}},
    {1, {0, 0, 0, 0},  {}, grey100_col},
    {1, {0, 0, 0, 0},  {},  grey60_col},
    {1, {0, 3, 0, 0},  {}, grey100_col, {default_col,  yellow_col,   default_col,   default_col}},
    {1, {0, 0, 0, 0}},
    {1, {0, 0, 0, 0}},
    {1, {0, 0, 0, 0}},
    {1, {0, 0, 0, 0}},
    {1, {0, 0, 0, 0}},
    {1, {0, 2, 0, 0}},
    {},
    {},
    {},
    {},
    {},
    {},
    // Y = 3
    {1, {0, 0, 0, 1},  {}, grey100_col, {default_col, default_col,   default_col, off_white_col}},
    {1, {0, 0, 0, 0},  {},  grey60_col},
    {1, {0, 0, 0, 0},  {}, grey100_col},
    {1, {0, 1, 0, 0},  {},  grey60_col, {default_col,  yellow_col,   default_col,   default_col}},
    {1, {0, 0, 0, 0}},
    {1, {0, 0, 0, 0}},
    {1, {0, 0, 0, 0}},
    {1, {0, 0, 0, 0}},
    {1, {0, 0, 0, 0}},
    {1, {0, 1, 0, 0}},
    {},
    {},
    {},
    {},
    {},
    {},
    // Y = 4
    {1, {0, 0, 1, 1},  {},   green_col, {default_col, default_col,      cyan_col,      cyan_col}},
    {1, {0, 0, 3, 0},  {},   green_col, {default_col, default_col,      cyan_col,   default_col}},
    {1, {0, 0, 1, 0},  {},   green_col, {default_col, default_col,      cyan_col,   default_col}},
    {1, {0, 1, 1, 0},  {},   green_col, {default_col,  yellow_col,      cyan_col,   default_col}},
    {1, {0, 0, 0, 0}},
    {1, {0, 0, 0, 0}},
    {1, {0, 0, 0, 0}},
    {1, {0, 0, 0, 0}},
    {1, {0, 0, 0, 0}},
    {1, {0, 3, 0, 0}},
    {},
    {},
    {},
    {},
    {},
    {},
    // Y = 5
    {1, {0, 0, 0, 1},  {},   green_col, {default_col, default_col,      cyan_col,      cyan_col}},
    {1, {0, 0, 0, 0},  {},   green_col, {default_col, default_col,      cyan_col,   default_col}},
    {1, {0, 0, 0, 0},  {},   green_col, {default_col, default_col,      cyan_col,   default_col}},
    {1, {0, 1, 0, 0},  {},   green_col, {default_col,  yellow_col,      cyan_col,   default_col}},
    {1, {0, 0, 0, 0}},
    {1, {0, 0, 0, 0}},
    {1, {0, 0, 0, 0}},
    {1, {0, 0, 0, 0}},
    {1, {0, 0, 0, 0}},
    {1, {0, 1, 0, 0}},
    {},
    {},
    {},
    {},
    {},
    {},
    // Y = 6
    {1, {0, 0, 0, 1},  {},   green_col, {default_col, default_col,      cyan_col,      cyan_col}},
    {1, {0, 0, 0, 0},  {},   green_col, {default_col, default_col,      cyan_col,   default_col}},
    {1, {0, 0, 0, 0},  {},   green_col, {default_col, default_col,      cyan_col,   default_col}},
    {1, {0, 3, 0, 0},  {},   green_col, {default_col,  yellow_col,      cyan_col,   default_col}},
    {1, {0, 0, 0, 0}},
    {1, {0, 0, 0, 0}},
    {1, {0, 0, 0, 0}},
    {1, {0, 0, 0, 0}},
    {1, {0, 0, 0, 0}},
    {1, {0, 2, 0, 0}},
    {},
    {},
    {},
    {},
    {},
    {},
    // Y = 7
    {1, {0, 0, 0, 2},  {},   green_col, {default_col, default_col,      cyan_col,      cyan_col}},
    {1, {0, 0, 0, 0},  {},   green_col, {default_col, default_col,      cyan_col,   default_col}},
    {1, {0, 0, 0, 0},  {},   green_col, {default_col, default_col,      cyan_col,   default_col}},
    {1, {0, 1, 0, 0},  {},   green_col, {default_col,  yellow_col,      cyan_col,   default_col}},
    {1, {0, 0, 0, 0}},
    {1, {0, 0, 0, 0}},
    {1, {0, 0, 0, 0}},
    {1, {0, 0, 0, 0}},
    {1, {0, 0, 0, 0}},
    {1, {0, 1, 0, 0}},
    {},
    {},
    {},
    {},
    {},
    {},
    // Y = 8
    {1, {1, 0, 0, 1},  {},   green_col, {default_col, default_col,      cyan_col,      cyan_col}},
    {1, {1, 0, 0, 0},  {},   green_col, {default_col, default_col,      cyan_col,   default_col}},
    {1, {2, 0, 0, 0},  {},   green_col, {default_col, default_col,      cyan_col,   default_col}},
    {1, {1, 1, 0, 0},  {},   green_col, {default_col,  yellow_col,      cyan_col,   default_col}},
    {1, {1, 0, 0, 0}},
    {1, {1, 0, 0, 0}},
    {1, {1, 0, 0, 0}},
    {1, {2, 0, 0, 0}},
    {1, {1, 0, 0, 0}},
    {1, {1, 1, 0, 0}},
    {},
    {},
    {},
    {},
    {},
    {},
};

World::World() : path_finder(*this) {
    tiles = blit::Surface::load(asset_iso_tile);

    scroll_pos = {blit::screen.bounds.w / 2, 40};

    // setup some defaults
    for(int y = 0; y < map_height; y++) {
        for(int x = 0; x < map_width; x++) {
            chunks[x + y * map_width].x = x;
            chunks[x + y * map_width].y = y;
        }
    }

    memcpy(chunks[0].tiles, default_chunk, sizeof(default_chunk));

    // put some "grass" outside
    for(auto &chunk : chunks) {
        for(auto &tile : chunk.tiles) {
            if(tile.floor == 0) {
                tile.floor = 2;
                tile.floor_colour = {25, 133, 25};
            }
        }
    }

    auto &entities = chunks[0].entities;

    // test entity data
    entities.emplace_back(*this, entities.size(), blit::Point{0, 0}, entity_bathroom_shower); // shower
    entities.emplace_back(*this, entities.size(), blit::Point{2, 0}, entity_bathroom_toilet); // toilet
    entities.emplace_back(*this, entities.size(), blit::Point{6, 0}, entity_kitchen_oven); // oven
    entities.emplace_back(*this, entities.size(), blit::Point{7, 0}, entity_kitchen_sink); // kitchen sink
    entities.emplace_back(*this, entities.size(), blit::Point{8, 0}, entity_kitchen_fridge); // fridge

    entities.emplace_back(*this, entities.size(), blit::Point{2, 3}, entity_bathroom_sink, 2); // bathroom sink

    entities.emplace_back(*this, entities.size(), blit::Point{1, 6}, entity_bedroom_bed, 1); // bed

    entities.emplace_back(*this, entities.size(), blit::Point{9, 7}, entity_livingroom_tv, 3); // tv

    auto test_person_index = entities.size();
    entities.emplace_back(*this, test_person_index, blit::Point{1, 1}, Person::entity_info); // test person
    people.emplace_back(*this, test_person_index);
}

void World::render() {
    using blit::screen;
    using blit::Point;

    Point offset = get_scroll_offset();

    screen.sprites = tiles;

    // colours from the floor/walls may be reused, so save the original values
    blit::Pen orig_cols[9];
    memcpy(orig_cols, tiles->palette, sizeof(orig_cols));

    auto draw_sprite = [](Point pos, const SpriteInfo &sprite){
        screen.sprite({sprite.sheet_x, sprite.sheet_y, sprite.sheet_w, sprite.sheet_h}, {pos.x - sprite.center_x, pos.y - sprite.center_y}, sprite.transform);
    };

    auto draw_wall = [this, &draw_sprite, &orig_cols](Point pos, const MapTile &tile, WallSide side) {
        if(tile.walls[side]) {
            int base_sprite = tile.walls[side];
            if(walls_hidden)
                base_sprite = wall_to_hidden[base_sprite];

            // colour multipliers for shading (8-bit fraction, -1 so a scale of 1.0 will fit)
            const uint8_t col_mul_r[]{255, 162, 144,  92};
            const uint8_t col_mul_g[]{255, 162, 164,  99};
            const uint8_t col_mul_b[]{255, 159, 172, 108};

            blit::Pen wall_col{99, 102, 99};

            if(tile.wall_colour[side].a)
                wall_col = tile.wall_colour[side];

            screen.sprites->palette[5 + side] = {
                wall_col.r * (col_mul_r[side] + 1) / 256,
                wall_col.g * (col_mul_g[side] + 1) / 256,
                wall_col.b * (col_mul_b[side] + 1) / 256
            };

            // also set base col (if reusing same sprite for all walls)
            screen.sprites->palette[5] = screen.sprites->palette[5 + side];

            // recolour window/door frames
            for(int i = 2; i < 5; i++) {
                auto col = screen.sprites->palette[i];
                screen.sprites->palette[i] = {
                    col.r * (col_mul_r[side] + 1) / 256,
                    col.g * (col_mul_g[side] + 1) / 256,
                    col.b * (col_mul_b[side] + 1) / 256
                };
            }

            draw_sprite(pos, sprites[base_sprite * 4 + side]);

            screen.sprites->palette[5 + side] = orig_cols[5 + side];

            for(int i = 2; i < 6; i++)
                screen.sprites->palette[i] = orig_cols[i];
        }
    };

    // TODO: chunks around camera
    // TODO: draw order
    for(auto &chunk : chunks) {
        for(int x = 0; x < chunk_width; x++) {
            for(int y = 0; y < chunk_height; y++) {
                auto center_pos = to_screen_pos(x + chunk.x * chunk_width, y + chunk.y * chunk_width) + offset;

                auto &tile = chunk.tiles[x + y * chunk_width];

                // 0 could be a valid floor sprite, but not a wall sprite (as it's the floor)

                // top/left walls
                draw_wall(center_pos, tile, Side_Top);
                draw_wall(center_pos, tile, Side_Left);

                // floor
                if(tile.floor) {
                    blit::Pen floor_col{154, 128, 103};
                    if(tile.floor_colour.a)
                        floor_col = tile.floor_colour;

                    tiles->palette[1] = floor_col;
                    tiles->palette[2] = {floor_col.r * 234 / 256, floor_col.g * 234 / 256, floor_col.b * 234 / 256}; // grid lines
                    tiles->palette[3] = {floor_col.r * 156 / 256, floor_col.g * 156 / 256, floor_col.b * 156 / 256}; // shading
                    tiles->palette[4] = {floor_col.r * 103 / 256, floor_col.g * 103 / 256, floor_col.b * 103 / 256}; // more shading

                    draw_sprite(center_pos, sprites[tile.floor - 1]);

                    for(int i = 1; i < 5; i++)
                        tiles->palette[i] = orig_cols[i];
                }

                for(auto ent_id : tile.entities) {
                    if(ent_id && ent_id != 0xFF) {
                        auto &ent = chunk.entities[ent_id - 1];
                        draw_sprite(center_pos - Point(tile_width / 2, tile_height / 2) + ent.get_offset_in_tile(), sprites[ent.get_sprite_index()]);
                    }
                }

                draw_wall(center_pos, tile, Side_Bottom);
            }
            
            // draw right walls after drawing entire line (for objects that cover multiple tiles on the y axis)
            // multiple x tiles is a bit broken
            for(int y = 0; y < chunk_height; y++) {
                auto center_pos = to_screen_pos(x + chunk.x * chunk_width, y + chunk.y * chunk_width) + offset;

                auto &tile = chunk.tiles[x + y * chunk_width];

                draw_wall(center_pos, tile, Side_Right);
            }
        }
    }
}

void World::update(uint32_t time) {
    minute_timer++; // called from update, so 10ms
    if(minute_timer == 100) {
        minute_timer = 0;
        this->time++;
    }

    path_finder.update();

    for(auto &person : people)
        person.update(time);
}

unsigned int World::create_entity(blit::Point tile_pos, const EntityInfo &info, int rotation) {
    unsigned int new_id = 0;

    // check first
    blit::Size ent_size(info.w, info.h);
    if(rotation == 1 || rotation == 3)
        std::swap(ent_size.w, ent_size.h);

    if(!can_place_entity(tile_pos, ent_size, 0))
        return ~0;

    // lookup chunk
    auto chunk = get_chunk_from_tile(tile_pos);

    if(!chunk)
        return ~0;

    auto &entities = chunk->entities;

    for(auto &ent : entities) {
        // "removed" entity, reuse
        if(ent.get_position() == blit::Point{-16, -16} && &ent.get_info() == &info) {
            ent.set_rotation(rotation);
            ent.set_tile_position(tile_pos);
            return make_global_entity_id(new_id, chunk->x, chunk->y);
        }

        new_id++;
    }

    // create new
    new_id = make_global_entity_id(new_id, chunk->x, chunk->y);
    entities.emplace_back(*this, new_id, tile_pos, info, rotation);

    return new_id;
}

void World::destroy_entity(unsigned int entity) {
    int chunk_x, chunk_y;
    auto local_id = get_local_entity_id(entity, chunk_x, chunk_y);

    // lookup chunk
    auto chunk = get_chunk({chunk_x, chunk_y});

    if(!chunk)
        return;

    auto &entities = chunk->entities;

    if(local_id >= entities.size())
        return;

    if(local_id == entities.size() - 1) {
        remove_entity(entities.back().get_tile_position(), entities.back().get_size(), entity);
        entities.pop_back();
    } else 
        entities[local_id].set_position({-16, -16}); // "remove"
}

bool World::add_entity(blit::Point tile_pos, blit::Size ent_size, unsigned int entity) {
    // lookup chunk
    int chunk_x, chunk_y;
    auto local_id = get_local_entity_id(entity, chunk_x, chunk_y);
    auto chunk = get_chunk_from_tile(tile_pos);

    if(!chunk)
        return false;
    
    // check that pos and entity id are consistent
    if(chunk->x != chunk_x || chunk->y != chunk_y)
        return false;

    // local tile pos
    blit::Point local_pos(tile_pos.x % chunk_width, tile_pos.y % chunk_height);

    blit::Rect map_rect(0, 0, chunk_width, chunk_height);

    // check full entity fits
    if(!map_rect.contains(local_pos - blit::Point(ent_size.w - 1, ent_size.h - 1)))
        return false;

    auto &base_tile = chunk->tiles[local_pos.x + local_pos.y * chunk_width];

    for(size_t i = 0; i < std::size(base_tile.entities); i++) {
        // already here
        if(base_tile.entities[i] == local_id + 1)
            return true;

        if(can_place_entity(tile_pos, ent_size, i)) {
            uint8_t id = local_id + 1;

            for(int y = 0; y < ent_size.h; y++) {
                for(int x = 0; x < ent_size.w; x++) {
                    chunk->tiles[local_pos.x - x + (local_pos.y - y) * chunk_width].entities[i] = id;
                    id = 0xFF; // only use the entity id on the first tile
                }
            }

            return true;
        }

        // this tile is free, one of the others isn't
        // stop here to not stack across tiles
        if(!base_tile.entities[i])
            return false;
    }

    // we're full
    return false;
}

bool World::remove_entity(blit::Point tile_pos, blit::Size ent_size, unsigned int entity) {
    // lookup chunk
    int chunk_x, chunk_y;
    auto local_id = get_local_entity_id(entity, chunk_x, chunk_y);
    auto chunk = get_chunk_from_tile(tile_pos);

    if(!chunk)
        return false;
    
    // check that pos and entity id are consistent
    if(chunk->x != chunk_x || chunk->y != chunk_y)
        return false;

    // local tile pos
    blit::Point local_pos(tile_pos.x % chunk_width, tile_pos.y % chunk_height);

    auto &base_tile = chunk->tiles[local_pos.x + local_pos.y * chunk_width];

    for(size_t i = 0; i < std::size(base_tile.entities); i++) {
         if(base_tile.entities[i] == local_id + 1) {
            // clear all tiles
            for(int y = 0; y < ent_size.h; y++) {
                for(int x = 0; x < ent_size.w; x++) {
                    chunk->tiles[local_pos.x - x + (local_pos.y - y) * chunk_width].entities[i] = 0;
                }
            }
            return true;
         }
    }

    // wasn't here
    return false;
}

bool World::move_entity(blit::Point old_tile_pos, blit::Point new_tile_pos, blit::Size ent_size, unsigned int entity) {
    // fail if we can't add to the new tile
    if(!add_entity(new_tile_pos, ent_size, entity))
        return false;

    return remove_entity(old_tile_pos, ent_size, entity);
}

unsigned int World::find_entity(blit::Point tile_pos, const EntityInfo &info) {
    // lookup chunk
    auto chunk = get_chunk_from_tile(tile_pos);

    if(!chunk)
        return ~0;

    // lookup tile in chunk
    blit::Point local_pos(tile_pos.x % chunk_width, tile_pos.y % chunk_height);

    auto &tile = chunk->tiles[local_pos.x + local_pos.y * chunk_width];

    for(auto ent_id : tile.entities) {
        if(ent_id && ent_id != 0xFF && &chunk->entities[ent_id - 1].get_info() == &info)
            return ent_id - 1;
    }

    return ~0;
}

bool World::can_place_entity(blit::Point tile_pos, blit::Size ent_size, int index) const {
    // lookup chunk
    auto chunk = get_chunk_from_tile(tile_pos);

    if(!chunk)
        return false;

    // local tile pos
    blit::Point local_pos(tile_pos.x % chunk_width, tile_pos.y % chunk_height);

    blit::Rect map_rect(0, 0, chunk_width, chunk_height);

    // check all tiles
    for(int y = 0; y < ent_size.h; y++) {
        for(int x = 0; x < ent_size.w; x++) {
            // check map bounds
            if(!map_rect.contains({local_pos.x - x, local_pos.y - y}))
                return false;

            auto &tile = chunk->tiles[local_pos.x - x + (local_pos.y - y) * chunk_width];

            // check of this slot is empty AND that the previous slot isn't to avoid weird stacking
            if(tile.entities[index] || (index > 0 && !tile.entities[index - 1]))
                return false;

            // check walls
            if((x && tile.walls[Side_Right]) || (x < ent_size.w - 1 && tile.walls[Side_Left]))
                return false;
            if((y && tile.walls[Side_Bottom]) || (y < ent_size.h - 1 && tile.walls[Side_Top]))
                return false;
        }
    }

    return true;
}

Entity &World::get_entity(unsigned int entity) {
    int chunk_x, chunk_y;
    auto local_id = get_local_entity_id(entity, chunk_x, chunk_y);
    auto chunk = get_chunk({chunk_x, chunk_y});
    return chunk->entities[local_id];
}

std::vector<unsigned int> World::get_entities_for_need(Person::Need need) const {
    std::vector<unsigned int> ret;

    // TODO: don't search all chunks
    for(auto &chunk : chunks) {
        unsigned int index = 0;
        for(auto &entity : chunk.entities) {
            if(entity.get_info().need_effect[static_cast<int>(need)] > 0.0f) {
                // check if someone else is using it
                bool allow_multi_use = entity.get_info().use_range > 1; // TODO: add a flag for this

                auto ent_id = make_global_entity_id(index, chunk.x, chunk.y);

                if(allow_multi_use || !is_entity_in_use(ent_id))
                    ret.push_back(ent_id);
            }

            index++;
        }
    }

    return ret;
}

std::vector<unsigned int> World::get_entities_for_action(uint32_t action_mask) const {
    std::vector<unsigned int> ret;

    // TODO: don't search all chunks
    for(auto &chunk : chunks) {
        unsigned int index = 0;
        for(auto &entity : chunk.entities) {
            if(entity.get_info().basic_actions & action_mask) {
                // check if someone else is using it
                bool allow_multi_use = entity.get_info().use_range > 1; // TODO: add a flag for this

                auto ent_id = make_global_entity_id(index, chunk.x, chunk.y);

                if(allow_multi_use || !is_entity_in_use(ent_id))
                    ret.push_back(ent_id);
            }

            index++;
        }
    }

    return ret;
}

std::vector<unsigned int> World::get_entities_on_tile(blit::Point tile_pos) const {
    auto chunk = get_chunk_from_tile(tile_pos);

    if(!chunk)
        return {};

    std::vector<unsigned int> ret;

    blit::Point local_pos(tile_pos.x % chunk_width, tile_pos.y % chunk_height);

    auto &tile = chunk->tiles[local_pos.x + local_pos.y * chunk_width];

    int index = 0;
    for(auto ent : tile.entities) {
        if(ent == 0xFF)
            ent = find_real_entity_id(tile_pos, index);

        if(ent && ent != 0xFF)
            ret.push_back(make_global_entity_id(ent - 1, chunk->x, chunk->y));

        index++;
    }

    return ret;
}

bool World::has_entity_for_need(blit::Point tile_pos, Person::Need need) const {
    auto chunk = get_chunk_from_tile(tile_pos);

    if(!chunk)
        return false;

    blit::Point local_pos(tile_pos.x % chunk_width, tile_pos.y % chunk_height);

    auto &tile = chunk->tiles[local_pos.x + local_pos.y * chunk_width];

    int index = 0;
    for(auto ent : tile.entities) {
        if(ent == 0xFF)
            ent = find_real_entity_id(tile_pos, index);

        if(ent && ent != 0xFF && chunk->entities[ent - 1].get_info().need_effect[static_cast<int>(need)] > 0.0f)
            return true;

        index++;
    }

    return false;
}

bool World::has_entity_for_action(blit::Point tile_pos, uint32_t action_mask) const {
    auto chunk = get_chunk_from_tile(tile_pos);

    if(!chunk)
        return false;

    blit::Point local_pos(tile_pos.x % chunk_width, tile_pos.y % chunk_height);

    auto &tile = chunk->tiles[local_pos.x + local_pos.y * chunk_width];

    int index = 0;
    for(auto ent : tile.entities) {
        if(ent == 0xFF)
            ent = find_real_entity_id(tile_pos, index);

        if(ent && ent != 0xFF && chunk->entities[ent - 1].get_info().basic_actions & action_mask)
            return true;

        index++;
    }

    return false;
}

bool World::is_entity_in_use(unsigned int entity) const {
    for(auto &person : people) {
        if(person.is_using_entity(entity))
            return true;
    }

    return false;
}

size_t World::get_num_people() const {
    return people.size();
}

Person &World::get_person(unsigned int person) {
    return people[person];
}

Person *World::find_person(unsigned int entity) {
    for(auto &person : people) {
        if(person.get_entity_index() == entity)
            return &person;
    }

    return nullptr;
}

// checks if there is any person awake and not otherwise busy
bool World::check_people_available() const {
    for(auto &person : people) {
        if(person.is_sleeping())
            continue;

        return true;
    }

    return false;
}

PathFinder &World::get_path_finder() {
    return path_finder;
}

bool World::get_walls_hidden() const {
    return walls_hidden;
}

void World::set_walls_hidden(bool hidden) {
    walls_hidden = hidden;
}

MapTile *World::get_tile(int x, int y) {
    auto chunk = get_chunk_from_tile({x, y});

    if(!chunk)
        return nullptr;

    return &chunk->tiles[(x % chunk_width) + (y % chunk_height) * chunk_width];
}

blit::Size World::get_bounds() const {
    return {chunk_width * map_width, chunk_height * map_height};
}

blit::Point World::to_screen_pos(int x, int y, int z) const {
    return {x * tile_width / 2 - y * tile_width / 2, y * tile_height / 2 + x * tile_height / 2 - z};
}

blit::Point World::to_screen_pos(int x, int y, int z, blit::Point frac_offset) const {
    return to_screen_pos(x, y, z) - blit::Point(tile_width / 2, tile_height / 2) + frac_offset;
}

blit::Point World::from_screen_pos(blit::Point screen) const {
    return {
        (screen.x / 2 + screen.y + tile_height / 2) / tile_height,
        (-screen.x / 2 + screen.y + tile_height / 2) / tile_height
    };
}

blit::Point World::get_scroll_offset() const {
    return scroll_pos;
}

void World::scroll_to_tile(int x, int y, int z) {
    // TODO: offset for status bar?
    blit::Point screen_center(blit::screen.bounds.w / 2, blit::screen.bounds.h / 2);
    auto tile_screen_pos = to_screen_pos(x, y, z);

    scroll_pos = screen_center - tile_screen_pos;
}

void  World::scroll_to_tile(int x, int y, int z, blit::Point frac_offset) {
    // TODO: offset for status bar?
    blit::Point screen_center(blit::screen.bounds.w / 2, blit::screen.bounds.h / 2);
    auto tile_screen_pos = to_screen_pos(x, y, z, frac_offset);

    scroll_pos = screen_center - tile_screen_pos;
}

World::Time World::get_time(uint32_t offset) const {

    int offset_timer = minute_timer + offset;
    uint32_t offset_time = time + (offset_timer / 100);
    offset_timer %= 100;

    uint8_t seconds = offset_timer * 60 / 100;
    uint8_t minutes = offset_time % 60;
    uint8_t hours = (offset_time / 60) % 24;
    uint32_t days = offset_time / (24 * 60);
    uint8_t day_of_week = days % 7;

    return {
        seconds,
        minutes,
        hours,
        day_of_week,
        days
    };
}

uint32_t World::get_timestamp() const {
    return time;  
}

unsigned int World::find_real_entity_id(blit::Point tile_pos, int index) const {
    auto check_pos = [this, index](int x, int y) -> unsigned int {
        auto chunk = get_chunk_from_tile({x, y});

        if(!chunk)
            return 0;

        auto ent = chunk->tiles[(x % chunk_width) + (y % chunk_height) * chunk_width].entities[index];
        if(!ent || ent == 0xFF)
            return 0;

        auto ent_size = chunk->entities[ent - 1].get_size();

        if(blit::Rect({x, y}, ent_size).contains({x, y}))
            return ent;
        
        return 0;
    };

    auto bounds = get_bounds();

    // so far biggest objects are 2x1/1x2, so just check adjacent tiles
    unsigned int ent;
    if(tile_pos.x > 0 && (ent = check_pos(tile_pos.x - 1, tile_pos.y)))
        return ent;

    if(tile_pos.x < bounds.w - 1 && (ent = check_pos(tile_pos.x + 1, tile_pos.y)))
        return ent;

    if(tile_pos.y > 0 && (ent = check_pos(tile_pos.x, tile_pos.y - 1)))
        return ent;

    if(tile_pos.y < bounds.h - 1 && (ent = check_pos(tile_pos.x, tile_pos.y + 1)))
        return ent;

    return 0xFF;
}

unsigned int World::make_global_entity_id(unsigned int id, int chunk_x, int chunk_y) const {
    return id | (chunk_x + chunk_y * chunk_width) << 8;
}

unsigned int World::get_local_entity_id(unsigned int id, int &chunk_x, int &chunk_y) const {
    chunk_x = (id >> 8) % chunk_width;
    chunk_y = (id >> 8) / chunk_width;

    return id & 0xFF;
}

World::Chunk *World::get_chunk(blit::Point chunk_pos) {
    return const_cast<World::Chunk *>(std::as_const(*this).get_chunk(chunk_pos));
}

const World::Chunk *World::get_chunk(blit::Point chunk_pos) const {
    if(chunk_pos.x < 0 || chunk_pos.y < 0)
        return nullptr;

    for(auto &chunk : chunks){
        if(chunk.x == chunk_pos.x && chunk.y == chunk_pos.y)
            return &chunk;
    }

    return nullptr;
}

World::Chunk *World::get_chunk_from_tile(blit::Point tile_pos) {
    return const_cast<World::Chunk *>(std::as_const(*this).get_chunk_from_tile(tile_pos));
}

const World::Chunk *World::get_chunk_from_tile(blit::Point tile_pos) const {
    if(tile_pos.x < 0 || tile_pos.y < 0)
        return nullptr;

    return get_chunk(tile_pos / chunk_width); // assuming square
}