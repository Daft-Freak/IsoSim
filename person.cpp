#include "person.hpp"
#include "world.hpp"

Person::Person(World &world, uint16_t entity_index) : world(world), entity_index(entity_index){

}

void Person::update(uint32_t time) {
    auto &entity = world.get_entity(entity_index);
    // test moving
    entity.set_position({72 + int(std::sin(time / 600.0f) * 52), 72 + int(std::cos(time / 600.0f) * 32)});
}
