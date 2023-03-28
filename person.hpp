#pragma once
#include <cstdint>

#include "path-finder.hpp"
#include "behaviour-tree.hpp"

class World;

class Person {
public:
    Person(World &world, uint16_t entity_index);

    void update(uint32_t time);

    bool is_moving() const;
    void move_to_tile(blit::Point tile_pos);

    uint16_t get_entity_index() const;

private:
    World &world;
    uint16_t entity_index;

    BehaviourTreeState behaviour_tree;

    blit::Point move_start_pos, move_target_pos;
    int move_progress;
};
