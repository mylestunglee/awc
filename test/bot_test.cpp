#define EXPOSE_BOT_INTERNALS
#include "../bot.h"
#include "../constants.h"
#include "game_fixture.hpp"
#include "test_constants.hpp"

TEST_F(game_fixture, find_attackee_maximises_metric) {
    insert_unit({.player = 0, .x = 2});
    units_select_at(&game->units, 2, 0);
    insert_unit({.health = HEALTH_MAX, .player = 1, .x = 3});
    insert_unit({.health = HEALTH_MAX / 2, .player = 2, .x = 5});
    game->labels[0][3] = ATTACKABLE_BIT;
    game->labels[0][5] = ATTACKABLE_BIT;

    auto attackee = find_attackee(game, units_const_get_at(&game->units, 2, 0));

    ASSERT_EQ(attackee, units_const_get_at(&game->units, 5, 0));
}
