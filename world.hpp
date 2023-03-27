#pragma once

#include <vector>

#include "graphics/surface.hpp"
#include "types/point.hpp"

#include "entity.hpp"
#include "path-finder.hpp"
#include "surface-helper.hpp"

struct MapTile {
    uint8_t floor;
    uint8_t walls[4]; // bottom, right, top, left
    uint8_t entities[2] = {};

    blit::Pen floor_colour = {};
    blit::Pen wall_colour[4] = {};
};

class World final {
public:
    World();

    void render();

    void update(uint32_t time);

    unsigned int create_entity(blit::Point tile_pos, const EntityInfo &info, int rotation = 0);
    void destroy_entity(unsigned int entity);

    // can return false if no room
    bool add_entity(blit::Point tile_pos, blit::Size ent_size, unsigned int entity);
    bool remove_entity(blit::Point tile_pos, blit::Size ent_size, unsigned int entity);

    unsigned int find_entity(blit::Point tile_pos, const EntityInfo &info);

    bool can_place_entity(blit::Point tile_pos, blit::Size ent_size, int index) const;

    bool get_walls_hidden() const;
    void set_walls_hidden(bool hidden);

    MapTile *get_tile(int x, int y);

    blit::Size get_bounds() const;

    blit::Point to_screen_pos(int x, int y, int z = 0) const;
    blit::Point from_screen_pos(blit::Point screen) const;

    blit::Point get_scroll_offset() const;

    uint32_t get_time() const;

private:
    static const int tile_width = 32, tile_height = 16;

    OwnedSurface tiles;

    static const int map_width = 16, map_height = 16;
    MapTile map[map_width * map_height];

    std::vector<Entity> entities;

    PathFinder path_finder;

    bool walls_hidden = false;

    int minute_timer = 0;
    uint32_t time = 0; // in minutes
};