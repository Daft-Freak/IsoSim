#pragma once

#include <memory>

#include "game-state.hpp"
#include "world.hpp"

class Game;

class Level final : public GameState {
public:
    Level(Game *game, std::shared_ptr<World> world = nullptr);
    ~Level() override;

    void update(uint32_t time) override;
    void render() override;

private:
    Game *game;

    std::shared_ptr<World> world;
};