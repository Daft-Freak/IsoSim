#include "engine/api.hpp"
#include "engine/engine.hpp"

#include "level.hpp"

#include "edit-mode.hpp"
#include "game.hpp"
#include "world.hpp"

static void format_time(uint32_t time, char *buf, size_t buf_size) {
    int minutes = time % 60;
    int hours = (time / 60) % 24;
    int days = time / (24 * 60);
    int day_of_week = days % 7;
    const char *day_names[]{"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

    snprintf(buf, buf_size, "%s %02i:%02i", day_names[day_of_week], hours, minutes);
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

    char buf[100];
    format_time(world->get_time(), buf, sizeof(buf));

    screen.pen = {0xFF, 0xFF, 0xFF};
    screen.text(buf, blit::minimal_font, {10, 10});
}
