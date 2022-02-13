#include "entity.hpp"


Entity::Entity(blit::Point pos, unsigned int sprite_index) : sprite_index(sprite_index) {
    set_tile_position(pos);
};

blit::Point Entity::get_position() const {
    return position;
}

void Entity::set_position(blit::Point p) {
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
    position = p * 16 + blit::Point(8, 8);
}

unsigned int Entity::get_sprite_index() const {
    return sprite_index;
}
