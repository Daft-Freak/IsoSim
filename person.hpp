#pragma once
#include <cstdint>

#include "path-finder.hpp"

class World;

class Person {
public:
    Person(World &world, uint16_t entity_index);

    void update(uint32_t time);

    bool is_moving() const;
    void move_to_tile(blit::Point tile_pos);

    // for debugging
    const PathFinder::Path &get_path() const {return path;}

private:
    World &world;
    uint16_t entity_index;

    PathFinder::Path path;
    unsigned int cur_path_point = 0;

    blit::Point move_start_pos, move_target_pos;
    int move_progress;
};
