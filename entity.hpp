#pragma once

#include "types/point.hpp"
#include "types/size.hpp"

class World;

class Entity {
public:
    Entity(World &world, uint16_t index, blit::Point pos, uint16_t sprite_index);

    blit::Point get_position() const;
    void set_position(blit::Point p);

    blit::Point get_tile_position() const;
    void set_tile_position(blit::Point p);

    blit::Point get_offset_in_tile() const;

    uint16_t get_sprite_index() const;

private:
    World &world;

    uint16_t index;
    uint16_t sprite_index = 0;

    blit::Point position; // 16ths of a tile
    blit::Size size = {1, 1};
};