#pragma once

#include "game-state.hpp"
#include "world.hpp"

class Game;

class EditMode final : public GameState {
public:
    EditMode(Game *game, World &&world);
    ~EditMode() override;

    void update(uint32_t time) override;
    void render() override;

private:
    Game *game;

    World world;

    blit::Point tile_cursor;
};