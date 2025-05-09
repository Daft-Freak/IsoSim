#include "engine/api.hpp"
#include "engine/engine.hpp"
#include "graphics/color.hpp"

#include "edit-mode.hpp"

#include "game.hpp"
#include "level.hpp"
#include "sprite-info.hpp"
#include "world.hpp"

EditMode::ObjectMenu::ObjectMenu() : blit::Menu("Objects") {
    item_h = 40;
    header_h = footer_h = 0;
    margin_y = 0;

    background_colour = blit::Pen(0x11, 0x11, 0x11);
    foreground_colour = blit::Pen(0xF7, 0xF7, 0xF7);
    selected_item_background = blit::Pen(0x22, 0x22, 0x22);

    num_items = wall_id_end / 4 + (object_id_end - object_id_start) / 4;
    auto new_items = new Item[num_items];

    // floor/wall
    int i = 0;
    for(int sprite_id = 0; sprite_id < wall_id_end; i++, sprite_id += 4) {
        new_items[i].label = "";
        new_items[i].id = sprite_id;
    }

    // objects
    for(int sprite_id = object_id_start; sprite_id < object_id_end; i++, sprite_id += 4) {
        new_items[i].label = "";
        new_items[i].id = sprite_id;
    }

    items = new_items;
}

EditMode::ObjectMenu::~ObjectMenu() {
    delete[] items;
}

void EditMode::ObjectMenu::set_on_item_activated(std::function<void(const Item &)> func) {
    on_item_pressed = func;
}

void EditMode::ObjectMenu::render_item(const Item &item, int y, int index) const {
    using blit::screen;

    blit::Menu::render_item(item, y, index);

    blit::Rect item_rect(display_rect.x, y, display_rect.w, item_h);

    auto clip = screen.clip;

    int sprite_index = item.id;

    // selected item
    if(index == current_item) {
        sprite_index += rotation;
    } else {
        screen.clip = screen.clip.intersection(item_rect);
        screen.alpha = 100;
    }

    // display sprite
    auto &sprite = sprites[sprite_index];
    auto pos = item_rect.center();
    screen.sprite({sprite.sheet_x, sprite.sheet_y, sprite.sheet_w, sprite.sheet_h}, {pos.x - (sprite.sheet_w * 8 / 2), pos.y - (sprite.sheet_h * 8 / 2)});

    // restore clip/alpha
    screen.clip = clip;
    screen.alpha = 0xFF;
}

void EditMode::ObjectMenu::update_menu(uint32_t time) {
    // for spinning the current item
    if(items[current_item].id < wall_id_start)
        rotation = 0;
    else if(--rotation_timer == 0)
    {
        rotation = (rotation + 1) % 4;
        rotation_timer = 100;
    }
}

void EditMode::ObjectMenu::item_activated(const Item &item) {
    if(on_item_pressed)
        on_item_pressed(item);
}

EditMode::EditMode(Game *game, std::shared_ptr<World> world) : game(game), world(world), menu("Edit Mode", {{Menu_SelectObject, "Select Object"}, {Menu_Exit, "Exit"}}) {

    int menu_w = blit::screen.bounds.w / 3;
    int menu_x = blit::screen.bounds.w - menu_w;
    menu.set_display_rect({menu_x, 0, menu_w, blit::screen.bounds.h});
    menu.set_on_item_activated(std::bind(&EditMode::on_menu_activated, this, std::placeholders::_1));

    object_menu.set_display_rect({menu_x, 0, menu_w, blit::screen.bounds.h});
    object_menu.set_on_item_activated(std::bind(&EditMode::on_object_menu_activated, this, std::placeholders::_1));
}

EditMode::~EditMode() {
}

void EditMode::update(uint32_t time) {

    if(blit::buttons.released & blit::Button::MENU)
        show_menu = !show_menu;

    if(show_object_menu) {
        object_menu.update(time);
        return;
    }

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

    world->scroll_to_tile(tile_cursor.x, tile_cursor.y);

    if(blit::buttons.released & blit::Button::Y)
        world->set_walls_hidden(!world->get_walls_hidden());

    // rotate
    if(blit::buttons.released & blit::Button::X) {
        if(cur_object >= wall_id_start)
            object_rotation = (object_rotation + 1) % 4;
    }

    // place object
    if(blit::buttons.released & blit::Button::A) {
        auto tile = world->get_tile(tile_cursor.x, tile_cursor.y);
        if(tile) {
            if(cur_object < wall_id_start)
                tile->floor = cur_object + 1;
            else if(cur_object < wall_id_end)
                tile->walls[object_rotation] = cur_object / 4;
            else if(cur_object >= object_id_start && cur_object < object_id_end) {
                auto &ent_info = sprite_to_entity(cur_object);
                auto ent = world->find_entity(tile_cursor, ent_info);

                if(ent == ~0u) // check if already there
                    world->create_entity(tile_cursor, ent_info, object_rotation);
            }
        }
    }

    // remove object
    if(blit::buttons.released & blit::Button::B) {
        auto tile = world->get_tile(tile_cursor.x, tile_cursor.y);
        if(tile) {
            if(cur_object < wall_id_start)
                tile->floor = 0;
            else if(cur_object < wall_id_end)
                tile->walls[object_rotation] = 0;
            else if(cur_object >= object_id_start && cur_object < object_id_end) {
                auto &ent_info = sprite_to_entity(cur_object);
                auto ent = world->find_entity(tile_cursor, ent_info);
                if(ent != ~0u)
                    world->destroy_entity(ent);
            }
        }
    }
}

void EditMode::render() {
    using blit::screen;

    screen.pen = {0x63, 0x9b, 0xff}; // "sky" colour
    screen.clear();

    world->render();

    if(show_object_menu) {
        object_menu.render();
        return;
    }

    if(show_menu) {
        menu.render();
        return;
    }

    auto pos = world->get_scroll_offset() + world->to_screen_pos(tile_cursor.x, tile_cursor.y);

    // object sprite
    auto sprite = sprites[cur_object + object_rotation];
    screen.alpha = 100;
    screen.sprite({sprite.sheet_x, sprite.sheet_y, sprite.sheet_w, sprite.sheet_h}, {pos.x - sprite.center_x, pos.y - sprite.center_y});
    screen.alpha = 0xFF;

    // get size of object (walls/floor are 1x1)
    blit::Size size(1, 1);
    bool can_place = true;

    if(cur_object >= object_id_start) {
        auto ent = sprite_to_entity(cur_object);
        size = {ent.w, ent.h};

        if(object_rotation == 1 || object_rotation == 3) // 90/270 deg rotation
            std::swap(size.w, size.h);

        can_place = world->can_place_entity(tile_cursor, size, 0);
    }

    // tile cursor
    if(can_place)
        screen.pen = {0, 255, 0};
    else
        screen.pen = {255, 0, 0};

    for(int y = 0; y < size.h; y++) {
        for(int x = 0; x < size.w; x++) {
            // TODO: un-hardcode/de-duplicate
            const int tile_w = 32;
            const int tile_h = 16;

            pos = world->get_scroll_offset() + world->to_screen_pos(tile_cursor.x - x, tile_cursor.y - y);

            screen.line(pos + blit::Point{-tile_w / 2,           0}, pos + blit::Point{          0, -tile_h / 2});
            screen.line(pos + blit::Point{          0, -tile_h / 2}, pos + blit::Point{ tile_w / 2,           0});
            screen.line(pos + blit::Point{ tile_w / 2,           0}, pos + blit::Point{          0,  tile_h / 2});
            screen.line(pos + blit::Point{          0,  tile_h / 2}, pos + blit::Point{-tile_w / 2,           0});
        }
    }
}

void EditMode::on_menu_activated(const Menu::Item &item) {
    switch(item.id) {
        case Menu_SelectObject:
            show_object_menu = true;
            break;

        case Menu_Exit:
            game->change_state<Level>(std::move(world));
            break;
    }
}

void EditMode::on_object_menu_activated(const Menu::Item &item) {
    show_object_menu = false;
    cur_object = item.id;

    // floor can't be rotated
    if(item.id < wall_id_start)
        object_rotation = 0;
}

const EntityInfo &EditMode::sprite_to_entity(int sprite_index) {
    // TODO: don't use sprite ids for menu?
    static const EntityInfo *all_entities[]{
        &entity_bathroom_shower,
        &entity_bathroom_sink,
        &entity_bathroom_toilet,

        &entity_bedroom_bed,

        &entity_kitchen_fridge,
        &entity_kitchen_oven,
        &entity_kitchen_sink,

        &entity_livingroom_tv,
    };

    for(auto ent_info : all_entities) {
        if(ent_info->base_sprite == sprite_index)
            return *ent_info;
    }

    return entity_kitchen_sink;
}