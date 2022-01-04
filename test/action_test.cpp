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
    ASSERT_TRUE(unit);
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

TEST_F(game_fixture, action_attack_where_direct_unit_with_counter_damage) {
    insert_unit({.health = health_max, .x = 2, .y = 3, .enabled = true});
    insert_unit({.health = health_max, .x = 5, .y = 7});
    units_select_at(&game->units, 2, 3);
    game->prev_x = 4;
    game->prev_y = 7;
    game->x = 5;
    game->y = 7;
    game->map[7][4] = tile_plains;
    game->map[7][5] = tile_plains;
    game->labels[7][5] = attackable_bit;
    game->dirty_labels = true;

    action_attack(game);

    const struct unit* const attacker = units_const_get_at(&game->units, 4, 7);
    const struct unit* const attackee = units_const_get_at(&game->units, 5, 7);
    ASSERT_TRUE(attacker);
    ASSERT_FALSE(attacker->enabled);
    ASSERT_LT(attacker->health, health_max);
    ASSERT_GT(attacker->health, attackee->health);
    ASSERT_FALSE(units_has_selection(&game->units));
}

TEST_F(game_fixture, action_attack_where_ranged_unit_kills) {
    constexpr model_t artillery = 5;
    insert_unit({.health = health_max,
                 .model = artillery,
                 .x = 2,
                 .y = 3,
                 .enabled = true});
    insert_unit({.health = 1, .x = 5, .y = 7});
    units_select_at(&game->units, 2, 3);
    game->x = 5;
    game->y = 7;
    game->map[7][5] = tile_plains;
    game->labels[7][5] = attackable_bit;
    game->dirty_labels = true;

    action_attack(game);

    ASSERT_FALSE(units_const_get_at(&game->units, 5, 7));
    ASSERT_FALSE(units_has_selection(&game->units));
}

TEST_F(game_fixture, action_attack_where_counter_damage_kills) {
    insert_unit({.health = 1, .x = 2, .y = 3, .enabled = true});
    insert_unit({.health = health_max, .x = 5, .y = 7});
    units_select_at(&game->units, 2, 3);
    game->prev_x = 4;
    game->prev_y = 7;
    game->x = 5;
    game->y = 7;
    game->map[7][4] = tile_plains;
    game->map[7][5] = tile_plains;
    game->labels[7][5] = attackable_bit;
    game->dirty_labels = true;

    action_attack(game);

    ASSERT_FALSE(units_const_get_at(&game->units, 4, 7));
    ASSERT_FALSE(units_has_selection(&game->units));
}
