#define EXPOSE_BOT_INTERNALS
#include "../bot.h"
#include "../constants.h"
#include "game_fixture.hpp"
#include "test_constants.hpp"

TEST_F(game_fixture, find_attackee_maximises_metric) {
    insert_selected_unit();
    insert_unit({.health = HEALTH_MAX, .x = 3});
    const auto* const expected_attackee =
        insert_unit({.health = HEALTH_MAX / 2, .x = 5});
    game->labels[0][3] = ATTACKABLE_BIT;
    game->labels[0][5] = ATTACKABLE_BIT;

    auto actual_attackee = find_attackee(game, MODEL_INFANTRY);

    ASSERT_EQ(actual_attackee, expected_attackee);
}

TEST_F(game_fixture, set_prev_position_maximises_defense) {
    auto* const attackee = insert_unit({.x = 3, .y = 3});
    game->labels[3][2] = ACCESSIBLE_BIT;
    game->labels[3][4] = ACCESSIBLE_BIT;
    game->map[3][2] = TILE_PLAINS;
    game->map[3][4] = TILE_FACTORY;
    game->energies[3][2] = 1;
    game->energies[3][4] = 1;

    set_prev_position(game, MODEL_INFANTRY, attackee);

    ASSERT_EQ(game->prev_x, 4);
    ASSERT_EQ(game->prev_y, 3);
}

TEST_F(game_fixture, prepare_attack_sets_position_for_ranged_unit) {
    const auto* const attackee = insert_unit({.x = 2, .y = 3});

    prepare_attack(game, MODEL_ARTILLERY, attackee);

    ASSERT_EQ(game->x, 2);
    ASSERT_EQ(game->y, 3);
}

TEST_F(game_fixture, prepare_attack_sets_position_for_direct_unit) {
    const auto* const attackee = insert_unit({.x = 2, .y = 3});
    game->labels[3][1] = ACCESSIBLE_BIT;
    game->energies[3][1] = 1;

    prepare_attack(game, MODEL_INFANTRY, attackee);

    ASSERT_EQ(game->prev_x, 1);
    ASSERT_EQ(game->prev_y, 3);
    ASSERT_EQ(game->x, 2);
    ASSERT_EQ(game->y, 3);
}

/*
TEST_F(game_fixture, handle_ranged_attack) {
    insert_unit({.health = HEALTH_MAX, .model = MODEL_ARTILLERY, .x = 2,
.enabled = true}); units_select_at(&game->units, 2, 0); insert_unit({.health =
HEALTH_MAX, .x = 3}); game->labels[0][3] = ATTACKABLE_BIT; game->dirty_labels =
true;

    handle_ranged_attack(game, units_const_get_at(&game->units, 3, 0));

    ASSERT_EQ(game->x, 3);
    auto attacker = units_const_get_at(&game->units, 2, 0);
    ASSERT_EQ(attacker->health, HEALTH_MAX);
}
*/
