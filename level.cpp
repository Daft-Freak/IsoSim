#include "engine/api.hpp"
#include "engine/engine.hpp"

#include "level.hpp"

#include "edit-mode.hpp"
#include "game.hpp"
#include "sprite-info.hpp"
#include "world.hpp"

static void format_time(const World::Time &time, char *buf, size_t buf_size) {
    const char *day_names[]{"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

    snprintf(buf, buf_size, "%s %02i:%02i", day_names[time.day_of_week], time.hours, time.minutes);
}

Level::Level(Game *game, std::shared_ptr<World> world) : game(game), world(world), menu("Live Mode", {{Menu_EditMode, "Edit mode"}}) {
    if(!world)
        this->world = std::make_shared<World>();

    int menu_w = blit::screen.bounds.w / 3;
    int menu_x = blit::screen.bounds.w - menu_w;
    menu.set_display_rect({menu_x, 0, menu_w, blit::screen.bounds.h});
    menu.set_on_item_activated(std::bind(&Level::on_menu_activated, this, std::placeholders::_1));
}

Level::~Level() {
}

void Level::update(uint32_t time) {

    if(blit::buttons.released & blit::Button::MENU)
        show_menu = !show_menu;

    if(show_menu) {
        menu.update(time);
        return;
    }

    if(blit::buttons.released & blit::Button::Y)
        world->set_walls_hidden(!world->get_walls_hidden());

    int speed = 1;

    // fast-forward through boring parts
    if(!world->check_people_available())
        speed = 20;

    for(int i = 0; i < speed; i++)
        world->update(time);
}

void Level::render() {
    using blit::screen;

    screen.pen = {0x63, 0x9b, 0xff}; // "sky" colour
    screen.clear();

    world->render();

    // time
    char buf[100];
    format_time(world->get_time(), buf, sizeof(buf));

    screen.pen = {0xFF, 0xFF, 0xFF};
    screen.text(buf, blit::minimal_font, {10, 10});

    // bg for info
    screen.pen = {0xFF, 0xFF, 0xFF, 0x80};
    screen.rectangle({0, 196, screen.bounds.w, 44});

    // needs
    auto &person = world->get_person(selected_person);

    auto draw_bar = [](blit::Point pos, float need) {
        screen.pen = {0, 0, 0};
        screen.rectangle({pos.x, pos.y, 50, 5});

        screen.pen = {255, 255, 255};
        int val = static_cast<int>(need * 50 + 0.5f);
        screen.rectangle({pos.x, pos.y, val, 5});
    };

    draw_bar({10, 200}, person.get_need(Person::Need::Sleep));
    draw_bar({10, 207}, person.get_need(Person::Need::Hunger));
    draw_bar({10, 214}, person.get_need(Person::Need::Hygiene));
    draw_bar({10, 221}, person.get_need(Person::Need::Toilet));
    draw_bar({10, 228}, person.get_need(Person::Need::Fun));

    // current action
    auto ent_in_use_id = person.get_entity_in_use();
    if(ent_in_use_id != ~0u) {
        auto &sprite = sprites[world->get_entity(ent_in_use_id).get_sprite_base_index()];

        blit::Point pos(screen.bounds.w - 28, 228);
        screen.sprite({sprite.sheet_x, sprite.sheet_y, sprite.sheet_w, sprite.sheet_h}, {pos.x - (sprite.sheet_w * 8 / 2), pos.y - sprite.center_y});
    }

    screen.pen = {0, 0, 0};
    screen.text(person.get_action_label(), blit::minimal_font, {screen.bounds.w - 2, screen.bounds.h}, true, blit::TextAlign::bottom_right);

    if(show_menu)
        menu.render();
}

void Level::on_menu_activated(const Menu::Item &item) {
    switch(item.id) {
        case Menu_EditMode:
            game->change_state<EditMode>(std::move(world));
            break;
    }
}
