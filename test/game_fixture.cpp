#include "game_fixture.hpp"

game_fixture::game_fixture()
    : managed_game(std::make_unique<struct game>()), game(managed_game.get()) {
    game_initialise(game);
}

void game_fixture::insert_unit(const struct unit& unit) {
    units_insert(&game->units, &unit);
}
