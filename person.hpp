#pragma once
#include <cstdint>

#include "path-finder.hpp"
#include "behaviour-tree.hpp"

class World;

class Person {
public:
    enum class Need {
        Sleep = 0,
        Hunger,
        Hygiene,
        Toilet,
        Fun,
    };

    Person(World &world, uint16_t entity_index);

    void update(uint32_t time);

    bool is_moving() const;
    void move_to_tile(blit::Point tile_pos);

    const char *get_action_label() const;

    uint16_t get_entity_index() const;

    float &get_need(Need need);
    Need get_lowest_need() const;

    bool start_using_entity(unsigned int entity);
    bool stop_using_entity(unsigned int entity);
    bool is_using_entity(unsigned int entity) const;
    unsigned int get_entity_in_use() const;

private:
    World &world;
    uint16_t entity_index;

    BehaviourTreeState behaviour_tree;

    blit::Point move_start_pos, move_target_pos;
    int move_progress;

    float needs[5];

    unsigned int entity_in_use = ~0u;
};
