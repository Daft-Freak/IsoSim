#pragma once
#include <cstdint>

class World;

class Person {
public:
    Person(World &world, uint16_t entity_index);

    void update(uint32_t time);

private:
    World &world;
    uint16_t entity_index;
};
