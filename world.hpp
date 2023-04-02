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

    unsigned int find_entity(blit::Point tile_pos, const EntityInfo &info);

    bool can_place_entity(blit::Point tile_pos, blit::Size ent_size, int index) const;

    Entity &get_entity(unsigned int entity);
    std::vector<unsigned int> get_entities_for_need(Person::Need need) const;
    std::vector<unsigned int> get_entities_on_tile(blit::Point tile_pos) const;

    bool has_entity_for_need(blit::Point tile_pos, Person::Need need) const;

    Person &get_person(unsigned int person);
    Person *find_person(unsigned int entity);

    bool check_people_available() const;

    PathFinder &get_path_finder();

    bool get_walls_hidden() const;
    void set_walls_hidden(bool hidden);

    MapTile *get_tile(int x, int y);

    blit::Size get_bounds() const;

    blit::Point to_screen_pos(int x, int y, int z = 0) const;
    blit::Point from_screen_pos(blit::Point screen) const;

    blit::Point get_scroll_offset() const;

    Time get_time(uint32_t offset = 0) const;
    uint32_t get_timestamp() const;

private:
    static const int tile_width = 32, tile_height = 16;

    OwnedSurface tiles;

    static const int map_width = 16, map_height = 16;
    MapTile map[map_width * map_height];

    std::vector<Entity> entities;

    std::vector<Person> people;

    PathFinder path_finder;

    bool walls_hidden = false;

    int minute_timer = 0;
    uint32_t time = 0; // in minutes
};