#include "engine/api.hpp"
#include "engine/engine.hpp"

#include "level.hpp"

#include "game.hpp"
#include "world.hpp"

Level::Level(Game *game) : game(game) {
}

Level::~Level() {
}

void Level::update(uint32_t time) {

    if(blit::buttons.released & blit::Button::Y)
        world.set_walls_hidden(!world.get_walls_hidden());

    world.update(time);
}

void Level::render() {
    using blit::screen;

    screen.pen = {0x63, 0x9b, 0xff}; // "sky" colour
    screen.clear();

    world.render();
}
