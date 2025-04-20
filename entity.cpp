#include "entity.hpp"
#include "world.hpp"

Entity::Entity(World &world, unsigned int index, blit::Point pos, const EntityInfo &info, int rotation) : world(world), index(index), sprite_index(0), info(info) {
    sprite_index = info.base_sprite + rotation;

    position = {-16, -16};
    set_tile_position(pos);
}

blit::Point Entity::get_position() const {
    return position;
}

void Entity::set_position(blit::Point p) {
    auto old_pos = get_tile_position();

    if((p / 16) != old_pos) {
        if(!world.move_entity(old_pos, p / 16, get_size(), index))
            return;
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

// unrotated sprite
uint16_t Entity::get_sprite_base_index() const {
    return info.base_sprite;
}

int Entity::get_rotation() const {
    return sprite_index - info.base_sprite;
}

void Entity::set_rotation(int rotation) {
    sprite_index = info.base_sprite + rotation;
}

const EntityInfo &Entity::get_info() const {
    return info;
}

blit::Size Entity::get_size() const {
    blit::Size ret(info.w, info.h);

    if(get_rotation() & 1)
        std::swap(ret.w, ret.h);

    return ret;
}