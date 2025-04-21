#pragma once

#include <vector>

#include "graphics/surface.hpp"
#include "types/point.hpp"

#include "entity.hpp"
#include "person.hpp"
#include "path-finder.hpp"
#include "surface-helper.hpp"

struct MapTile {
    uint8_t floor;
    uint8_t walls[4]; // bottom, right, top, left
    uint8_t entities[2] = {};

    blit::Pen floor_colour = {};
    blit::Pen wall_colour[4] = {};
};

class World final {
public:
    struct Time {
        uint8_t seconds, minutes, hours;
        uint8_t day_of_week;
        uint32_t days;        
    };

    World();

    void render();

    void update(uint32_t time);

    unsigned int create_entity(blit::Point tile_pos, const EntityInfo &info, int rotation = 0);
    void destroy_entity(unsigned int entity);

    // can return false if no room
    bool add_entity(blit::Point tile_pos, blit::Size ent_size, unsigned int entity);
    bool remove_entity(blit::Point tile_pos, blit::Size ent_size, unsigned int entity);

    bool move_entity(blit::Point old_tile_pos, blit::Point new_tile_pos, blit::Size ent_size, unsigned int entity);

    unsigned int find_entity(blit::Point tile_pos, const EntityInfo &info);

    bool can_place_entity(blit::Point tile_pos, blit::Size ent_size, int index) const;

    Entity &get_entity(unsigned int entity);
    std::vector<unsigned int> get_entities_for_need(Person::Need need) const;
    std::vector<unsigned int> get_entities_for_action(uint32_t action_mask) const;
    std::vector<unsigned int> get_entities_on_tile(blit::Point tile_pos) const;

    bool has_entity_for_need(blit::Point tile_pos, Person::Need need) const;
    bool has_entity_for_action(blit::Point tile_pos, uint32_t action_mask) const;

    bool is_entity_in_use(unsigned int entity) const;

    size_t get_num_people() const;
    Person &get_person(unsigned int person);
    Person *find_person(unsigned int entity);

    bool check_people_available() const;

    PathFinder &get_path_finder();

    bool get_walls_hidden() const;
    void set_walls_hidden(bool hidden);

    MapTile *get_tile(int x, int y);

    bool is_same_chunk(blit::Point pos_a, blit::Point pos_b) const;

    blit::Size get_bounds() const;

    blit::Point to_screen_pos(int x, int y, int z = 0) const;
    blit::Point to_screen_pos(int x, int y, int z, blit::Point frac_offset) const;
    blit::Point from_screen_pos(blit::Point screen) const;

    blit::Point get_scroll_offset() const;

    void scroll_to_tile(int x, int y, int z = 0);
    void scroll_to_tile(int x, int y, int z, blit::Point frac_offset);

    Time get_time(uint32_t offset = 0) const;
    uint32_t get_timestamp() const;

private:
    static const int chunk_width = 16, chunk_height = 16;

    struct Chunk
    {
        uint8_t x, y;

        MapTile tiles[chunk_width * chunk_height];
        std::vector<Entity> entities;
    };

    unsigned int find_real_entity_id(blit::Point tile_pos, int index) const;

    unsigned int make_global_entity_id(unsigned int id, int chunk_x, int chunk_y) const;
    unsigned int get_local_entity_id(unsigned int id, int &chunk_x, int &chunk_y) const;

    Chunk *get_chunk(blit::Point chunk_pos);
    const Chunk *get_chunk(blit::Point chunk_pos) const;
    Chunk *get_chunk_from_tile(blit::Point tile_pos);
    const Chunk *get_chunk_from_tile(blit::Point tile_pos) const;

    static const int tile_width = 32, tile_height = 16;

    OwnedSurface tiles;

    blit::Point scroll_pos;

    static const int map_width = 3, map_height = 3;
    Chunk chunks[map_width * map_height];

    std::vector<Person> people;

    PathFinder path_finder;

    bool walls_hidden = false;

    int minute_timer = 0;
    uint32_t time = 0; // in minutes
};