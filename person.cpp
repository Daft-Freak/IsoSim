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

        cur_path_point = 0;
    } else if(cur_path_progress < 32) {
        cur_path_progress++;
    } else {
        cur_path_point++;
        cur_path_progress = 0;
    }

    // move along path
    if(!path.path.empty()) {
        if(cur_path_point + 1 == path.path.size())
            entity.set_tile_position(path.path[cur_path_point]);
        else {
            // center of tile in 16ths
            auto cur_pos = path.path[cur_path_point] * 16 + blit::Point(8, 8);
            auto next_pos = path.path[cur_path_point + 1] * 16 + blit::Point(8, 8);
            entity.set_position(cur_pos + (next_pos - cur_pos) * cur_path_progress / 32);
        }
    }
}
