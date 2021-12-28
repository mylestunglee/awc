#include "game_fixture.hpp"

game_fixture::game_fixture() : game(new struct game) {
    game_initialise(game);
}

game_fixture::~game_fixture() { delete game; }

void game_fixture::insert_unit(const struct unit& unit) {
    units_insert(&game->units, &unit);
}
