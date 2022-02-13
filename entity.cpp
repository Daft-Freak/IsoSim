#include "entity.hpp"
#include "world.hpp"

Entity::Entity(World &world, uint16_t index, blit::Point pos, uint16_t sprite_index) : world(world), index(index), sprite_index(sprite_index) {
    set_tile_position(pos);
};

blit::Point Entity::get_position() const {
    return position;
}

void Entity::set_position(blit::Point p) {
    auto old_pos = get_tile_position();
    if((p / 16) != old_pos) {
        world.add_entity(p / 16, index); // TODO: check this and reject move?
        world.remove_entity(old_pos, index);
    }

    position = p;
}

blit::Point Entity::get_tile_position() const {
    return position / 16;
}

blit::Point Entity::get_offset_in_tile() const {
    // assuming 32x16
    int x = position.x % 16;
    int y = position.y % 16;
    return {x - y + 16, (x + y) / 2};
}

void Entity::set_tile_position(blit::Point p) {
    set_position(p * 16 + blit::Point(8, 8));
}

uint16_t Entity::get_sprite_index() const {
    return sprite_index;
}
