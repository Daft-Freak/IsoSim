#pragma once

#include "game-state.hpp"
#include "surface-helper.hpp"

class Game;

class Level final : public GameState {
public:
    Level(Game *game);
    ~Level() override;

    void update(uint32_t time) override;
    void render() override;

private:
    Game *game;

    OwnedSurface tiles;
};