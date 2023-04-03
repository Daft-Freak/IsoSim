#pragma once

#include <memory>

#include "game-state.hpp"
#include "menu.hpp"
#include "world.hpp"

class Game;

class Level final : public GameState {
public:
    Level(Game *game, std::shared_ptr<World> world = nullptr);
    ~Level() override;

    void update(uint32_t time) override;
    void render() override;

private:
    enum MenuItem {
        Menu_EditMode = 0,
    };

    void on_menu_activated(const Menu::Item &item);

    Game *game;

    std::shared_ptr<World> world;

    Menu menu;
    bool show_menu = false;

    unsigned int selected_person = 0;
};