#pragma once

#include "game-state.hpp"
#include "menu.hpp"
#include "world.hpp"

class Game;

class EditMode final : public GameState {
public:
    EditMode(Game *game, World &&world);
    ~EditMode() override;

    void update(uint32_t time) override;
    void render() override;

private:
    enum MenuItem {
        Menu_Exit = 0,
    };

    void on_menu_activated(const Menu::Item &item);

    Game *game;

    World world;

    Menu menu;
    bool show_menu = false;

    blit::Point tile_cursor;
};