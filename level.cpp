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
    { 0, 5, 4, 2, 16,  8}, // floor
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

    // placeholder thing
    { 0, 0, 2, 2, 8, 16},
};

static const int map_width = 10, map_height = 9;
static const MapTile map[map_width * map_height]{
    {1, {0, 0, 1, 1}, 16}, {1, {0, 0, 1, 0}}, {1, {0, 0, 2, 0}, 16}, {1, {0, 1, 1, 0}}, {1, {0, 0, 1, 0}}, {1, {0, 0, 1, 0}}, {1, {0, 0, 1, 0}, 16}, {1, {0, 0, 2, 0}, 16}, {1, {0, 0, 1, 0}, 16}, {1, {0, 1, 1, 0}},
    {1, {0, 0, 0, 1}}, {1, {0, 0, 0, 0}}, {1, {0, 0, 0, 0}}, {1, {0, 1, 0, 0}}, {1, {0, 0, 0, 0}}, {1, {0, 0, 0, 0}}, {1, {0, 0, 0, 0}}, {1, {0, 0, 0, 0}}, {1, {0, 0, 0, 0}}, {1, {0, 1, 0, 0}},
    {1, {0, 0, 0, 1}}, {1, {0, 0, 0, 0}}, {1, {0, 0, 0, 0}}, {1, {0, 3, 0, 0}}, {1, {0, 0, 0, 0}}, {1, {0, 0, 0, 0}}, {1, {0, 0, 0, 0}}, {1, {0, 0, 0, 0}}, {1, {0, 0, 0, 0}}, {1, {0, 2, 0, 0}},
    {1, {0, 0, 0, 1}}, {1, {0, 0, 0, 0}}, {1, {0, 0, 0, 0}, 16}, {1, {0, 1, 0, 0}}, {1, {0, 0, 0, 0}}, {1, {0, 0, 0, 0}}, {1, {0, 0, 0, 0}}, {1, {0, 0, 0, 0}}, {1, {0, 0, 0, 0}}, {1, {0, 1, 0, 0}},
    {1, {0, 0, 1, 1}}, {1, {0, 0, 3, 0}}, {1, {0, 0, 1, 0}}, {1, {0, 1, 1, 0}}, {1, {0, 0, 0, 0}}, {1, {0, 0, 0, 0}}, {1, {0, 0, 0, 0}}, {1, {0, 0, 0, 0}}, {1, {0, 0, 0, 0}}, {1, {0, 3, 0, 0}},
    {1, {0, 0, 0, 1}}, {1, {0, 0, 0, 0}}, {1, {0, 0, 0, 0}}, {1, {0, 1, 0, 0}}, {1, {0, 0, 0, 0}}, {1, {0, 0, 0, 0}}, {1, {0, 0, 0, 0}}, {1, {0, 0, 0, 0}}, {1, {0, 0, 0, 0}}, {1, {0, 1, 0, 0}},
    {1, {0, 0, 0, 1}}, {1, {0, 0, 0, 0}, 16}, {1, {0, 0, 0, 0}}, {1, {0, 3, 0, 0}}, {1, {0, 0, 0, 0}}, {1, {0, 0, 0, 0}}, {1, {0, 0, 0, 0}}, {1, {0, 0, 0, 0}}, {1, {0, 0, 0, 0}}, {1, {0, 2, 0, 0}},
    {1, {0, 0, 0, 2}}, {1, {0, 0, 0, 0}}, {1, {0, 0, 0, 0}}, {1, {0, 1, 0, 0}}, {1, {0, 0, 0, 0}}, {1, {0, 0, 0, 0}}, {1, {0, 0, 0, 0}}, {1, {0, 0, 0, 0}}, {1, {0, 0, 0, 0}}, {1, {0, 1, 0, 0}},
    {1, {1, 0, 0, 1}}, {1, {1, 0, 0, 0}}, {1, {2, 0, 0, 0}}, {1, {1, 1, 0, 0}}, {1, {1, 0, 0, 0}}, {1, {1, 0, 0, 0}}, {1, {1, 0, 0, 0}}, {1, {2, 0, 0, 0}, 16}, {1, {1, 0, 0, 0}}, {1, {1, 1, 0, 0}},
};

Level::Level(Game *game) : game(game) {
    tiles = blit::Surface::load(asset_iso_tile);
}

Level::~Level() {
}

void Level::update(uint32_t time) {

}

void Level::render() {
    using blit::screen;

    screen.pen = {0x63, 0x9b, 0xff}; // "sky" colour
    screen.clear();

    int offset_x = screen.bounds.w / 2;
    int offset_y = 40;

    screen.sprites = tiles;

    auto draw_sprite = [](int x, int y, const SpriteInfo &sprite){
        screen.sprite({sprite.sheet_x, sprite.sheet_y, sprite.sheet_w, sprite.sheet_h}, {x - sprite.center_x, y - sprite.center_y});
    };

    for(int x = 0; x < map_width; x++) {
        for(int y = 0; y < map_height; y++) {
            int center_x = offset_x + x * tile_width / 2 - y * tile_width / 2;
            int center_y = offset_y + y * tile_height / 2 + x * tile_height / 2; // - z

            // palette swap
            if((x + y) & 1)
                tiles->palette[3] = {0x20, 0x20, 0x20};
            else
                tiles->palette[3] = {0xDF, 0xDF, 0xDF};

            auto &tile = map[x + y * map_width];

            // 0 could be a valid floor sprite, but not a wall sprite (as it's the floor)

            // top wall
            if(tile.walls[Side_Top])
                draw_sprite(center_x, center_y, sprites[tile.walls[Side_Top] * 4 + Side_Top]);

            // left wall
            if(tile.walls[Side_Left])
                draw_sprite(center_x, center_y, sprites[tile.walls[Side_Left] * 4 + Side_Left]);

            // floor
            if(tile.floor)
                draw_sprite(center_x, center_y, sprites[tile.floor - 1]);

            if(tile.object)
                draw_sprite(center_x, center_y, sprites[tile.object]);

            // bottom wall
            if(tile.walls[Side_Bottom])
                draw_sprite(center_x, center_y, sprites[tile.walls[Side_Bottom] * 4 + Side_Bottom]);

            // right wall
            if(tile.walls[Side_Right])
                draw_sprite(center_x, center_y, sprites[tile.walls[Side_Right] * 4 + Side_Right]);
        }
    }
}
