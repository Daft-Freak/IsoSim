#include "engine/engine.hpp"

#include "person.hpp"
#include "world.hpp"

Person::Person(World &world, uint16_t entity_index) : world(world), entity_index(entity_index){

}

void Person::update(uint32_t time) {
    auto &entity = world.get_entity(entity_index);

    if(cur_path_point + 1 >= path.path.size()) {
        while(true) {
            PathFinder::State path_state;
            auto bounds = world.get_bounds();

            blit::Point end_pos(blit::random() % bounds.w, blit::random() % bounds.h);

            while(!world.get_path_finder().find_path(entity.get_tile_position(), end_pos, path_state, path));

            if(!path.path.empty())
                break;
        }

        cur_path_point = 1;
        move_to_tile(path.path[cur_path_point]);
    }

    // move
    if(is_moving()) {
        move_progress++;
        entity.set_position(move_start_pos + (move_target_pos - move_start_pos) * move_progress / 32);
    } else {
        // next in path
        cur_path_point++;
        if(cur_path_point < path.path.size())
            move_to_tile(path.path[cur_path_point]);
    }
}

bool Person::is_moving() const {
    return move_start_pos != move_target_pos && world.get_entity(entity_index).get_position() != move_target_pos;
}

void Person::move_to_tile(blit::Point tile_pos) {
    move_target_pos = tile_pos * 16 + blit::Point(8, 8);
    move_start_pos = world.get_entity(entity_index).get_position();

    move_progress = 0;
}

uint16_t Person::get_entity_index() const {
    return entity_index;
}