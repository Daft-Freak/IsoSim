#include <cstring>

#include "engine/engine.hpp"

#include "world.hpp"

#include "assets.hpp"
#include "sprite-info.hpp"

// TODO: this is getting silly, need something to edit this with
static const blit::Pen default_col{}, yellow_col{255, 255, 128}, cyan_col{200, 255, 255}, green_col{50, 100, 50},
                       off_white_col{240, 240, 240}, grey60_col{60, 60, 60}, grey100_col{100, 100, 100};

World::World() : map{
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
} {
    tiles = blit::Surface::load(asset_iso_tile);

    // test entity data
    entities.emplace_back(*this, entities.size(), blit::Point{0, 0}, 44); // shower
    entities.emplace_back(*this, entities.size(), blit::Point{2, 0}, 52); // toilet
    entities.emplace_back(*this, entities.size(), blit::Point{6, 0}, 36); // oven
    entities.emplace_back(*this, entities.size(), blit::Point{7, 0}, 32); // kitchen sink
    entities.emplace_back(*this, entities.size(), blit::Point{8, 0}, 40); // fridge

    entities.emplace_back(*this, entities.size(), blit::Point{2, 3}, 50); // bathroom sink

    entities.emplace_back(*this, entities.size(), blit::Point{1, 6}, 25); // bed

    entities.emplace_back(*this, entities.size(), blit::Point{9, 7}, 31); // tv

    entities.emplace_back(*this, entities.size(), blit::Point{1, 1}, 56); // test thing
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
        screen.sprite({sprite.sheet_x, sprite.sheet_y, sprite.sheet_w, sprite.sheet_h}, {pos.x - sprite.center_x, pos.y - sprite.center_y});
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

            draw_sprite(pos, sprites[base_sprite * 4 + side]);

            screen.sprites->palette[5 + side] = orig_cols[5 + side];
        }
    };

    for(int x = 0; x < map_width; x++) {
        for(int y = 0; y < map_height; y++) {
            auto center_pos = to_screen_pos(x, y) + offset;

            auto &tile = map[x + y * map_width];

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
                if(ent_id) {
                    auto &ent = entities[ent_id - 1];
                    draw_sprite(center_pos - Point(tile_width / 2, tile_height / 2) + ent.get_offset_in_tile(), sprites[ent.get_sprite_index()]);
                }
            }

            draw_wall(center_pos, tile, Side_Bottom);
        }
        
        // draw right walls after drawing entire line (for objects that cover multiple tiles on the y axis)
        // multiple x tiles is a bit broken
        for(int y = 0; y < map_height; y++) {
            auto center_pos = to_screen_pos(x, y) + offset;

            auto &tile = map[x + y * map_width];

            draw_wall(center_pos, tile, Side_Right);
        }
    }
}

void World::update(uint32_t time) {
    // test moving
    entities[8].set_position({72 + int(std::sin(time / 600.0f) * 52), 72 + int(std::cos(time / 600.0f) * 32)});
}

bool World::add_entity(blit::Point tile_pos, unsigned int entity) {
    auto &tile = map[tile_pos.x + tile_pos.y * map_width];

    for(auto &ent_id : tile.entities) {
        if(!ent_id) {
            ent_id = entity + 1;
            return true;
        } else if(ent_id == entity + 1)
            return true; // already here
    }

    // we're full
    return false;
}

bool World::remove_entity(blit::Point tile_pos, unsigned int entity) {
    auto &tile = map[tile_pos.x + tile_pos.y * map_width];

    for(auto &ent_id : tile.entities) {
         if(ent_id == entity + 1) {
            ent_id = 0;
            return true;
         }
    }

    // wasn't here
    return false;
}

bool World::get_walls_hidden() const {
    return walls_hidden;
}

void World::set_walls_hidden(bool hidden) {
    walls_hidden = hidden;
}

MapTile *World::get_tile(int x, int y) {
    if(x < 0 || y < 0 || x >= map_width || y >= map_height)
        return nullptr;

    return &map[x + y * map_width];
}

blit::Point World::to_screen_pos(int x, int y, int z) const {
    return {x * tile_width / 2 - y * tile_width / 2, y * tile_height / 2 + x * tile_height / 2 - z};
}

blit::Point World::from_screen_pos(blit::Point screen) const {
    return {
        (screen.x / 2 + screen.y + tile_height / 2) / tile_height,
        (-screen.x / 2 + screen.y + tile_height / 2) / tile_height
    };
}

blit::Point World::get_scroll_offset() const {
    return {blit::screen.bounds.w / 2, 40};
}