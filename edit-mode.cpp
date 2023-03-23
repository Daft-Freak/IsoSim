#include "engine/api.hpp"
#include "engine/engine.hpp"
#include "graphics/color.hpp"

#include "edit-mode.hpp"

#include "game.hpp"
#include "level.hpp"
#include "world.hpp"

EditMode::EditMode(Game *game, World &&world) : game(game), world(std::move(world)) {
}

EditMode::~EditMode() {
}

void EditMode::update(uint32_t time) {

    if(blit::buttons.released & blit::Button::MENU) {
        game->change_state<Level>(std::move(world));
        return;
    }

    world.update(time);
}

void EditMode::render() {
    using blit::screen;

    screen.pen = {0x63, 0x9b, 0xff}; // "sky" colour
    screen.clear();

    world.render();
}
