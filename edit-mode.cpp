#include "engine/api.hpp"
#include "engine/engine.hpp"
#include "graphics/color.hpp"

#include "edit-mode.hpp"

#include "game.hpp"
#include "level.hpp"
#include "world.hpp"

EditMode::EditMode(Game *game, World &&world) : game(game), world(std::move(world)), menu("Edit Mode", {{Menu_Exit, "Exit"}}) {

    int menu_w = blit::screen.bounds.w / 3;
    int menu_x = blit::screen.bounds.w - menu_w;
    menu.set_display_rect({menu_x, 0, menu_w, blit::screen.bounds.h});
    menu.set_on_item_activated(std::bind(&EditMode::on_menu_activated, this, std::placeholders::_1));
}

EditMode::~EditMode() {
}

void EditMode::update(uint32_t time) {

    if(blit::buttons.released & blit::Button::MENU)
        show_menu = !show_menu;

    if(show_menu) {
        menu.update(time);
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

    if(blit::buttons.released & blit::Button::Y)
        world.set_walls_hidden(!world.get_walls_hidden());

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

    if(show_menu) {
        menu.render();
        return;
    }

    // tile cursor
    auto pos = world.get_scroll_offset() + world.to_screen_pos(tile_cursor.x, tile_cursor.y);

    screen.pen = {255, 0, 0};
    screen.line(pos + blit::Point{-16,  0}, pos + blit::Point{  0, -8});
    screen.line(pos + blit::Point{  0, -8}, pos + blit::Point{ 16,  0});
    screen.line(pos + blit::Point{ 16,  0}, pos + blit::Point{  0,  8});
    screen.line(pos + blit::Point{  0,  8}, pos + blit::Point{-16,  0});
}

void EditMode::on_menu_activated(const Menu::Item &item) {
    switch(item.id) {
        case Menu_Exit:
            game->change_state<Level>(std::move(world));
            break;
    }
}