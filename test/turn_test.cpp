#define expose_turn_internals
#include "../turn.h"
#include "game_fixture.hpp"
#include <cstdio>
#include <fstream>

TEST_F(game_fixture, repair_units_increases_unit_health) {
    insert_unit({.x = 2, .y = 3});
    game->territory[3][2] = game->turn;
    repair_units(game);
    ASSERT_EQ(units_const_get_first(&game->units, game->turn)->health,
              heal_rate);
    ASSERT_EQ(game->golds[game->turn], -gold_scale * heal_rate / health_max);
}

TEST_F(game_fixture, repair_units_ignores_unit_not_on_territory) {
    insert_unit({});
    repair_units(game);
    ASSERT_EQ(game->golds[game->turn], 0);
}

TEST_F(game_fixture, repair_units_caps_at_maximum_health) {
    insert_unit({.health = health_max - 1, .x = 2, .y = 3});
    game->territory[3][2] = game->turn;
    repair_units(game);
    ASSERT_EQ(units_const_get_first(&game->units, game->turn)->health,
              health_max);
    ASSERT_EQ(game->golds[game->turn], -gold_scale / health_max);
}