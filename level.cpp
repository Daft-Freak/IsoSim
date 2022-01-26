#include <cstring>

#include "engine/api.hpp"
#include "engine/engine.hpp"

#include "level.hpp"

#include "assets.hpp"
#include "game.hpp"

struct SpriteInfo {
    uint8_t sheet_x, sheet_y, sheet_w, sheet_h; // in 8x8 tiles
    uint8_t center_x, center_y; // in pixels
};

struct MapTile {
    uint8_t floor;
    uint8_t walls[4]; // bottom, right, top, left
    uint8_t object = 0;

    blit::Pen floor_colour = {};
    blit::Pen wall_colour[4] = {};
};

enum WallSide {
    Side_Bottom = 0,
    Side_Right,
    Side_Top,
    Side_Left
};

// base tile size
static const int tile_width = 32, tile_height = 16;

static const SpriteInfo sprites[] {
    { 0, 4, 4, 3, 16, 16}, // floor
    {},
    {}, // don't need to rotate the floor
    {},

    // wall
    { 4, 0, 2, 7, 16, 48}, // bottom
    { 6, 0, 2, 7,  0, 48}, // right
    { 8, 0, 2, 7,  0, 48}, // top
    {10, 0, 2, 7, 16, 48}, // left

    // wall + window
    {12, 0, 2, 7, 16, 48},
    {14, 0, 2, 7,  0, 48},
    {16, 0, 2, 7,  0, 48},
    {18, 0, 2, 7, 16, 48},

    // wall + door
    {20, 0, 2, 7, 16, 48},
    {22, 0, 2, 7,  0, 48},
    {24, 0, 2, 7,  0, 48},
    {26, 0, 2, 7, 16, 48},

    // hidden wall
    {28, 0, 2, 2, 16,  8},
    {30, 0, 2, 2,  0,  8},
    {28, 2, 2, 2,  0, 16},
    {30, 2, 2, 2, 16, 16},
    // hidden wall + door
    {28, 4, 2, 2, 16,  8},
    {30, 4, 2, 2,  0,  8},
    {28, 6, 2, 2,  0, 16},
    {30, 6, 2, 2, 16, 16},

    // placeholder thing
    { 0, 0, 2, 2, 8, 16},
};

static const uint8_t wall_to_hidden[]{
    0, // floor
    4, // wall
    4, // wall + window
    5, // wall + door
};

// TODO: this is getting silly, need something to edit this with
static const blit::Pen default_col{}, yellow_col{255, 255, 128}, cyan_col{200, 255, 255}, green_col{50, 100, 50},
                       off_white_col{240, 240, 240}, grey60_col{60, 60, 60}, grey100_col{100, 100, 100};

static const int map_width = 10, map_height = 9;
static const MapTile map[map_width * map_height]{
    // Y = 0
    {1, {0, 0, 1, 1}, 24,  grey60_col, {default_col, default_col, off_white_col, off_white_col}},
    {1, {0, 0, 1, 0},  0, grey100_col, {default_col, default_col, off_white_col,   default_col}},
    {1, {0, 0, 2, 0}, 24,  grey60_col, {default_col, default_col, off_white_col,   default_col}},
    {1, {0, 1, 1, 0},  0, grey100_col, {default_col,  yellow_col, off_white_col,   default_col}},
    {1, {0, 0, 1, 0},  0, default_col, {default_col, default_col,    yellow_col,   default_col}},
    {1, {0, 0, 1, 0},  0, default_col, {default_col, default_col,    yellow_col,   default_col}},
    {1, {0, 0, 1, 0}, 24, default_col, {default_col, default_col,    yellow_col,   default_col}},
    {1, {0, 0, 2, 0}, 24, default_col, {default_col, default_col,    yellow_col,   default_col}},
    {1, {0, 0, 1, 0}, 24, default_col, {default_col, default_col,    yellow_col,   default_col}},
    {1, {0, 1, 1, 0},  0, default_col, {default_col, default_col,    yellow_col,   default_col}},
    // Y = 1
    {1, {0, 0, 0, 1},  0, grey100_col, {default_col, default_col,   default_col, off_white_col}},
    {1, {0, 0, 0, 0},  0,  grey60_col},
    {1, {0, 0, 0, 0},  0, grey100_col},
    {1, {0, 1, 0, 0},  0,  grey60_col, {default_col,  yellow_col,   default_col,   default_col}},
    {1, {0, 0, 0, 0},  0},
    {1, {0, 0, 0, 0},  0},
    {1, {0, 0, 0, 0},  0},
    {1, {0, 0, 0, 0},  0},
    {1, {0, 0, 0, 0},  0},
    {1, {0, 1, 0, 0},  0},
    // Y = 2
    {1, {0, 0, 0, 1},  0,  grey60_col, {default_col, default_col,   default_col, off_white_col}},
    {1, {0, 0, 0, 0},  0, grey100_col},
    {1, {0, 0, 0, 0},  0,  grey60_col},
    {1, {0, 3, 0, 0},  0, grey100_col, {default_col,  yellow_col,   default_col,   default_col}},
    {1, {0, 0, 0, 0},  0},
    {1, {0, 0, 0, 0},  0},
    {1, {0, 0, 0, 0},  0},
    {1, {0, 0, 0, 0},  0},
    {1, {0, 0, 0, 0},  0},
    {1, {0, 2, 0, 0},  0},
    // Y = 3
    {1, {0, 0, 0, 1},  0, grey100_col, {default_col, default_col,   default_col, off_white_col}},
    {1, {0, 0, 0, 0},  0,  grey60_col},
    {1, {0, 0, 0, 0}, 24, grey100_col},
    {1, {0, 1, 0, 0},  0,  grey60_col, {default_col,  yellow_col,   default_col,   default_col}},
    {1, {0, 0, 0, 0},  0},
    {1, {0, 0, 0, 0},  0},
    {1, {0, 0, 0, 0},  0},
    {1, {0, 0, 0, 0},  0},
    {1, {0, 0, 0, 0},  0},
    {1, {0, 1, 0, 0},  0},
    // Y = 4
    {1, {0, 0, 1, 1},  0,   green_col, {default_col, default_col,      cyan_col,      cyan_col}},
    {1, {0, 0, 3, 0},  0,   green_col, {default_col, default_col,      cyan_col,   default_col}},
    {1, {0, 0, 1, 0},  0,   green_col, {default_col, default_col,      cyan_col,   default_col}},
    {1, {0, 1, 1, 0},  0,   green_col, {default_col,  yellow_col,      cyan_col,   default_col}},
    {1, {0, 0, 0, 0},  0},
    {1, {0, 0, 0, 0},  0},
    {1, {0, 0, 0, 0},  0},
    {1, {0, 0, 0, 0},  0},
    {1, {0, 0, 0, 0},  0},
    {1, {0, 3, 0, 0},  0},
    // Y = 5
    {1, {0, 0, 0, 1},  0,   green_col, {default_col, default_col,      cyan_col,      cyan_col}},
    {1, {0, 0, 0, 0},  0,   green_col, {default_col, default_col,      cyan_col,   default_col}},
    {1, {0, 0, 0, 0},  0,   green_col, {default_col, default_col,      cyan_col,   default_col}},
    {1, {0, 1, 0, 0},  0,   green_col, {default_col,  yellow_col,      cyan_col,   default_col}},
    {1, {0, 0, 0, 0},  0},
    {1, {0, 0, 0, 0},  0},
    {1, {0, 0, 0, 0},  0},
    {1, {0, 0, 0, 0},  0},
    {1, {0, 0, 0, 0},  0},
    {1, {0, 1, 0, 0},  0},
    // Y = 6
    {1, {0, 0, 0, 1},  0,   green_col, {default_col, default_col,      cyan_col,      cyan_col}},
    {1, {0, 0, 0, 0}, 24,   green_col, {default_col, default_col,      cyan_col,   default_col}},
    {1, {0, 0, 0, 0},  0,   green_col, {default_col, default_col,      cyan_col,   default_col}},
    {1, {0, 3, 0, 0},  0,   green_col, {default_col,  yellow_col,      cyan_col,   default_col}},
    {1, {0, 0, 0, 0},  0},
    {1, {0, 0, 0, 0},  0},
    {1, {0, 0, 0, 0},  0},
    {1, {0, 0, 0, 0},  0},
    {1, {0, 0, 0, 0},  0},
    {1, {0, 2, 0, 0},  0},
    // Y = 7
    {1, {0, 0, 0, 2},  0,   green_col, {default_col, default_col,      cyan_col,      cyan_col}},
    {1, {0, 0, 0, 0},  0,   green_col, {default_col, default_col,      cyan_col,   default_col}},
    {1, {0, 0, 0, 0},  0,   green_col, {default_col, default_col,      cyan_col,   default_col}},
    {1, {0, 1, 0, 0},  0,   green_col, {default_col,  yellow_col,      cyan_col,   default_col}},
    {1, {0, 0, 0, 0},  0},
    {1, {0, 0, 0, 0},  0},
    {1, {0, 0, 0, 0},  0},
    {1, {0, 0, 0, 0},  0},
    {1, {0, 0, 0, 0},  0},
    {1, {0, 1, 0, 0},  0},
    // Y = 8
    {1, {1, 0, 0, 1},  0,   green_col, {default_col, default_col,      cyan_col,      cyan_col}},
    {1, {1, 0, 0, 0},  0,   green_col, {default_col, default_col,      cyan_col,   default_col}},
    {1, {2, 0, 0, 0},  0,   green_col, {default_col, default_col,      cyan_col,   default_col}},
    {1, {1, 1, 0, 0},  0,   green_col, {default_col,  yellow_col,      cyan_col,   default_col}},
    {1, {1, 0, 0, 0},  0},
    {1, {1, 0, 0, 0},  0},
    {1, {1, 0, 0, 0},  0},
    {1, {2, 0, 0, 0}, 24},
    {1, {1, 0, 0, 0},  0},
    {1, {1, 1, 0, 0},  0},
};

static bool walls_hidden = false;

Level::Level(Game *game) : game(game) {
    tiles = blit::Surface::load(asset_iso_tile);
}

Level::~Level() {
}

void Level::update(uint32_t time) {

    if(blit::buttons.released & blit::Button::Y)
        walls_hidden = !walls_hidden;
}

void Level::render() {
    using blit::screen;

    screen.pen = {0x63, 0x9b, 0xff}; // "sky" colour
    screen.clear();

    int offset_x = screen.bounds.w / 2;
    int offset_y = 40;

    screen.sprites = tiles;

    // colours from the floor/walls may be reused, so save the original values
    blit::Pen orig_cols[9];
    memcpy(orig_cols, tiles->palette, sizeof(orig_cols));

    auto draw_sprite = [](int x, int y, const SpriteInfo &sprite){
        screen.sprite({sprite.sheet_x, sprite.sheet_y, sprite.sheet_w, sprite.sheet_h}, {x - sprite.center_x, y - sprite.center_y});
    };

    auto draw_wall = [&draw_sprite, &orig_cols](int x, int y, const MapTile &tile, WallSide side) {
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

            draw_sprite(x, y, sprites[base_sprite * 4 + side]);

            screen.sprites->palette[5 + side] = orig_cols[5 + side];
        }
    };

    for(int x = 0; x < map_width; x++) {
        for(int y = 0; y < map_height; y++) {
            int center_x = offset_x + x * tile_width / 2 - y * tile_width / 2;
            int center_y = offset_y + y * tile_height / 2 + x * tile_height / 2; // - z

            auto &tile = map[x + y * map_width];

            // 0 could be a valid floor sprite, but not a wall sprite (as it's the floor)

            // top/left walls
            draw_wall(center_x, center_y, tile, Side_Top);
            draw_wall(center_x, center_y, tile, Side_Left);

            // floor
            if(tile.floor) {
                blit::Pen floor_col{154, 128, 103};
                if(tile.floor_colour.a)
                    floor_col = tile.floor_colour;

                tiles->palette[1] = floor_col;
                tiles->palette[2] = {floor_col.r * 234 / 256, floor_col.g * 234 / 256, floor_col.b * 234 / 256}; // grid lines
                tiles->palette[3] = {floor_col.r * 156 / 256, floor_col.g * 156 / 256, floor_col.b * 156 / 256}; // shading
                tiles->palette[4] = {floor_col.r * 103 / 256, floor_col.g * 103 / 256, floor_col.b * 103 / 256}; // more shading

                draw_sprite(center_x, center_y, sprites[tile.floor - 1]);

                for(int i = 1; i < 5; i++)
                    tiles->palette[i] = orig_cols[i];
            }

            if(tile.object)
                draw_sprite(center_x, center_y, sprites[tile.object]);

            // bottom/right walls
            draw_wall(center_x, center_y, tile, Side_Bottom);
            draw_wall(center_x, center_y, tile, Side_Right);
        }
    }
}
