#define expose_action_internals
#include "../action.h"
#include "game_fixture.hpp"

TEST(action_test, merge_health_returns_added_health) {
    ASSERT_EQ(merge_health(2, 3), 5);
}

TEST(action_test, merge_health_bounds_overflow) {
    ASSERT_EQ(merge_health(health_max, 1), health_max);
}

TEST_F(game_fixture, move_selected_unit_when_no_merge) {
    insert_unit({.health = health_max, .x = 2, .y = 3, .enabled = true});
    units_select_at(&game->units, 2, 3);
    ASSERT_EQ(move_selected_unit(game, 5, 3), health_max);
    const struct unit* const unit = units_const_get_selected(&game->units);
    ASSERT_EQ(unit->x, 5);
    ASSERT_FALSE(unit->enabled);
}

TEST_F(game_fixture, move_selected_unit_when_merge_with_disabled_unit) {
    insert_unit({.health = 7, .x = 2, .y = 3, .enabled = true});
    insert_unit({.health = 11, .x = 5, .y = 3});
    units_select_at(&game->units, 2, 3);
    ASSERT_EQ(move_selected_unit(game, 5, 3), 7);
    ASSERT_EQ(game->units.size, 1);
}

TEST_F(game_fixture, move_selected_unit_when_merge_with_enabled_unit) {
    insert_unit({.health = 7, .x = 2, .y = 3, .enabled = true});
    insert_unit({.health = 11, .x = 5, .y = 3, .enabled = true});
    units_select_at(&game->units, 2, 3);
    ASSERT_EQ(move_selected_unit(game, 5, 3), 18);
    ASSERT_EQ(game->units.size, 1);
}

TEST_F(game_fixture,
       simulate_restricted_attack_scales_with_game_simulate_attack) {
    health_t health_max_even = health_max & '\xfe';
    insert_unit({.health = health_max_even, .x = 2, .y = 3});
    insert_unit({.x = 5, .y = 7});
    units_select_at(&game->units, 2, 3);
    game->x = 5;
    game->y = 7;

    health_t health_max_damage = 0;
    health_t counter_damage = 0;
    game_simulate_attack(game, &health_max_damage, &counter_damage);

    health_t health_half_damage = 0;
    simulate_restricted_attack(game, health_max / 2, &health_half_damage,
                               &counter_damage);

    ASSERT_EQ(health_max_damage / 2, health_half_damage);
}
