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

    if(blit::buttons.released & blit::Button::DPAD_LEFT)
        tile_cursor.x--;
    else if(blit::buttons.released & blit::Button::DPAD_RIGHT)
        tile_cursor.x++;

    if(blit::buttons.released & blit::Button::DPAD_UP)
        tile_cursor.y--;
    else if(blit::buttons.released & blit::Button::DPAD_DOWN)
        tile_cursor.y++;

    if(blit::buttons.released & blit::Button::A) {
        auto tile = world.get_tile(tile_cursor.x, tile_cursor.y);
        if(tile) {
            tile->floor = 1;
        }
    }

    if(blit::buttons.released & blit::Button::B) {
        auto tile = world.get_tile(tile_cursor.x, tile_cursor.y);
        if(tile) {
            tile->floor = 0;
        }
    }

    world.update(time);
}

void EditMode::render() {
    using blit::screen;

    screen.pen = {0x63, 0x9b, 0xff}; // "sky" colour
    screen.clear();

    world.render();

    // tile cursor
    auto pos = world.get_scroll_offset() + world.to_screen_pos(tile_cursor.x, tile_cursor.y);

    screen.pen = {255, 0, 0};
    screen.line(pos + blit::Point{-16,  0}, pos + blit::Point{  0, -8});
    screen.line(pos + blit::Point{  0, -8}, pos + blit::Point{ 16,  0});
    screen.line(pos + blit::Point{ 16,  0}, pos + blit::Point{  0,  8});
    screen.line(pos + blit::Point{  0,  8}, pos + blit::Point{-16,  0});
}
