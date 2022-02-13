#pragma once

#include "types/point.hpp"
#include "types/size.hpp"

class Entity {
public:
    Entity() = default;

    Entity(blit::Point pos, unsigned int sprite_index);

    blit::Point get_position() const;
    void set_position(blit::Point p);

    blit::Point get_tile_position() const;
    void set_tile_position(blit::Point p);

    blit::Point get_offset_in_tile() const;

    unsigned int get_sprite_index() const;

private:
    blit::Point position; // 16ths of a tile
    blit::Size size = {1, 1};

    unsigned int sprite_index = 0;
};