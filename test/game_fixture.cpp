#include "game_fixture.hpp"

game_fixture::game_fixture()
    : managed_game(std::make_unique<struct game>()), game(managed_game.get()) {
    game_initialise(game);
}

struct unit* game_fixture::insert_unit(const struct unit& unit) {
    assert(units_is_insertable(&game->units));
    units_insert(&game->units, &unit);
    return units_get_at(&game->units, unit.x, unit.y);
}

struct unit* game_fixture::insert_selected_unit(const struct unit& unit) {
    auto* inserted_unit = insert_unit(unit);
    units_select_at(&game->units, inserted_unit->x, inserted_unit->y);
    return inserted_unit;
}
