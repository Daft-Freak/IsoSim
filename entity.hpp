#pragma once

#include "types/point.hpp"
#include "types/size.hpp"

#include "entity-info.hpp"

class World;

class Entity {
public:
    Entity(World &world, uint16_t index, blit::Point pos, const EntityInfo &info, int rotation = 0);

    blit::Point get_position() const;
    void set_position(blit::Point p);

    blit::Point get_tile_position() const;
    void set_tile_position(blit::Point p);

    blit::Point get_offset_in_tile() const;

    uint16_t get_sprite_index() const;

    int get_rotation() const;
    void set_rotation(int rotation);

    const EntityInfo &get_info() const;

    blit::Size get_size() const;

private:
    World &world;

    uint16_t index;
    uint16_t sprite_index = 0;

    blit::Point position; // 16ths of a tile
    const EntityInfo &info;
};