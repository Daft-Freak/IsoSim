#include "engine/engine.hpp"

#include "game.hpp"

#include "level.hpp"

Game::Game() {
}

Game::~Game() {
    delete state;
    delete next_state;
}

void Game::update(uint32_t time) {
    if(state)
        state->update(time);

    // change state if requested
    if(next_state) {
        delete state;
        state = next_state;
        next_state = nullptr;
    }
}

void Game::render() {
    if(state)
        state->render();
}

static Game game;

void init() {
    blit::set_screen_mode(blit::ScreenMode::hires);
    game.change_state<Level>();
}

void render(uint32_t time) {
    game.render();
}

void update(uint32_t time) {
    game.update(time);
}