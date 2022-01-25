#include "engine/engine.hpp"

#include "level.hpp"

#include "assets.hpp"
#include "game.hpp"

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

    const int map_width = 10, map_height = 10;
    const int tile_width = 32, tile_height = 16;

    int offset_x = screen.bounds.w / 2;
    int offset_y = 24;

    screen.sprites = tiles;

    for(int x = 0; x < map_width; x++) {
        for(int y = 0; y < map_height; y++) {
            int center_x = offset_x + x * tile_width / 2 - y * tile_width / 2;
            int center_y = offset_y + y * tile_height / 2 + x * tile_height / 2; // - z

            // palette swap
            if((x + y) & 1)
                tiles->palette[3] = {0x20, 0x20, 0x20};
            else
                tiles->palette[3] = {0xDF, 0xDF, 0xDF};

            // walls
            int wall_y_off = tile_height + tile_height / 2;
            const int wall_h = 16;

            // top wall
            if(y < 5 && !(y & 1) && x >= y && y < map_width - x)
                screen.sprite({12, 0, tile_width / 8, wall_h * 2 / 8}, {center_x - tile_width / 2, center_y - wall_y_off});

            // left wall
            if(x < 5 && !(x & 1) && y >= x && x < map_height - y)
                screen.sprite({16, 0, tile_width / 8, wall_h * 2 / 8}, {center_x - tile_width / 2, center_y - wall_y_off});

            // floor
            screen.sprite({0, 2, tile_width / 8, tile_height / 8}, {center_x - tile_width / 2, center_y - tile_height / 2});

            // bottom wall
            if(y >= 5 && (y & 1) && x >= map_height - 1 - y && map_height - y <= map_width - x)
                screen.sprite({4, 0, tile_width / 8, wall_h * 2 / 8}, {center_x - tile_width / 2, center_y - wall_y_off});

            // right wall
            if(x >= 5 && (x & 1) && y >= map_width - 1 - x && map_width - x <= map_height - y)
                screen.sprite({8, 0, tile_width / 8, wall_h * 2 / 8}, {center_x - tile_width / 2, center_y - wall_y_off});
        }
    }
}
