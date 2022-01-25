#include "engine/engine.hpp"

#include "level.hpp"

#include "game.hpp"

Level::Level(Game *game) : game(game) {

}

Level::~Level() {
}

void Level::update(uint32_t time) {

}

void Level::render() {
    using blit::screen;

    screen.pen = {0x63, 0x9b, 0xff}; // "sky" colour
    screen.clear();
}
