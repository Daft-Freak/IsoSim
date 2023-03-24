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
        Menu_SelectObject = 0,
        Menu_Exit,
    };

    class ObjectMenu final : public blit::Menu {
    public:
        ObjectMenu();
        ~ObjectMenu();

        void set_on_item_activated(std::function<void(const Item &)> func);

    private:
        void render_item(const Item &item, int y, int index) const override;

        void item_activated(const Item &item) override;

        std::function<void(const Item &)> on_item_pressed = nullptr;
    };

    void on_menu_activated(const Menu::Item &item);

    void on_object_menu_activated(const Menu::Item &item);

    Game *game;

    World world;

    Menu menu;
    bool show_menu = false;

    ObjectMenu object_menu;
    bool show_object_menu = false;

    blit::Point tile_cursor;
};