#include "engine/api.hpp"
#include "engine/engine.hpp"

#include "level.hpp"

#include "edit-mode.hpp"
#include "game.hpp"
#include "world.hpp"

static void format_time(const World::Time &time, char *buf, size_t buf_size) {
    const char *day_names[]{"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

    snprintf(buf, buf_size, "%s %02i:%02i", day_names[time.day_of_week], time.hours, time.minutes);
}

Level::Level(Game *game, std::shared_ptr<World> world) : game(game), world(world) {
    if(!world)
        this->world = std::make_shared<World>();
}

Level::~Level() {
}

void Level::update(uint32_t time) {

    if(blit::buttons.released & blit::Button::Y)
        world->set_walls_hidden(!world->get_walls_hidden());

    if(blit::buttons.released & blit::Button::MENU) {
        game->change_state<EditMode>(world);
        return;
    }

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

    // needs
    auto &person = world->get_person(0);

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
}
