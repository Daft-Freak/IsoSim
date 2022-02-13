#pragma once

#include "graphics/surface.hpp"
#include "types/point.hpp"

#include "entity.hpp"
#include "surface-helper.hpp"

struct MapTile {
    uint8_t floor;
    uint8_t walls[4]; // bottom, right, top, left
    uint8_t entity = 0;

    blit::Pen floor_colour = {};
    blit::Pen wall_colour[4] = {};
};

class World final {
public:
    World();

    void render();

    void update(uint32_t time);

    // can return false if no room
    bool add_entity(blit::Point tile_pos, unsigned int entity);
    bool remove_entity(blit::Point tile_pos, unsigned int entity);

    bool get_walls_hidden() const;
    void set_walls_hidden(bool hidden);

    blit::Point to_screen_pos(int x, int y, int z = 0) const;
    blit::Point from_screen_pos(blit::Point screen) const;

private:
    static const int tile_width = 32, tile_height = 16;

    OwnedSurface tiles;

    static const int map_width = 10, map_height = 9;
    MapTile map[map_width * map_height];

    Entity entities[9] {
        {{0, 0}, 44}, // shower
        {{2, 0}, 52}, // toilet
        {{6, 0}, 36}, // oven
        {{7, 0}, 32}, // kitchen sink
        {{8, 0}, 40}, // fridge

        {{2, 3}, 50}, // bathroom sink

        {{1, 6}, 25}, // bed

        {{9, 7}, 31}, // tv

        {{1, 1}, 56}
    };

    bool walls_hidden = false;
};