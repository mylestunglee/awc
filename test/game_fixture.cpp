#include "game_fixture.hpp"

game_fixture::game_fixture() : game(new struct game) {
    memset(game, 0, sizeof(struct game));
    game_initialise(game);
}

game_fixture::~game_fixture() { delete game; }
