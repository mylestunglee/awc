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

TEST_F(game_fixture, action_build_returns_true_when_model_is_unbuildable) {
    game->golds[game->turn] = gold_scale;
    game->x = 2;
    game->y = 3;
    game->territory[3][2] = game->turn;
    game->map[3][2] = tile_city;
    constexpr model_t infantry = 0;

    ASSERT_TRUE(action_build(game, infantry));
}

TEST_F(game_fixture, action_build_returns_true_when_model_is_unaffordable) {
    game->x = 2;
    game->y = 3;
    game->territory[3][2] = game->turn;
    game->map[3][2] = tile_factory;
    constexpr model_t infantry = 0;

    ASSERT_TRUE(action_build(game, infantry));
}

TEST_F(game_fixture, action_build_returns_false_when_model_is_buildable) {
    game->golds[game->turn] = gold_scale;
    game->x = 2;
    game->y = 3;
    game->territory[3][2] = game->turn;
    game->map[3][2] = tile_factory;
    constexpr model_t infantry = 0;

    ASSERT_FALSE(action_build(game, infantry));

    const struct unit* const unit = units_const_get_at(&game->units, 2, 3);
    ASSERT_TRUE(unit);
    ASSERT_EQ(unit->health, health_max);
    ASSERT_EQ(unit->model, infantry);
    ASSERT_EQ(unit->player, game->turn);
    ASSERT_EQ(unit->x, 2);
    ASSERT_EQ(unit->y, 3);
    ASSERT_FALSE(unit->enabled);
    ASSERT_EQ(unit->capture_progress, 0);
}

TEST_F(game_fixture, can_selected_unit_capture_returns_true_when_capturable) {
    insert_unit({.x = 2, .y = 3});
    units_select_at(&game->units, 2, 3);
    game->x = 2;
    game->y = 3;
    game->map[3][2] = tile_city;
    game->territory[3][2] = 1;

    ASSERT_TRUE(can_selected_unit_capture(game));
}

TEST_F(game_fixture,
       can_selected_unit_capture_returns_false_when_uncapturable) {
    insert_unit({.x = 2, .y = 3});
    units_select_at(&game->units, 2, 3);
    game->x = 2;
    game->y = 3;

    ASSERT_FALSE(can_selected_unit_capture(game));
}

TEST_F(game_fixture, action_capture_when_capture_enemy_hq) {
    game->x = 2;
    game->y = 3;
    game->territory[3][2] = 1;
    game->map[3][2] = tile_hq;
    game->incomes[game->turn] = 5;
    game->incomes[1] = 5;

    action_capture(game);

    ASSERT_EQ(game->incomes[1], 0);
    ASSERT_EQ(game->territory[3][2], game->turn);
    ASSERT_EQ(game->incomes[game->turn], 6);
}

TEST_F(game_fixture, action_capture_when_capture_enemy_city) {
    game->x = 2;
    game->y = 3;
    game->territory[3][2] = 1;
    game->incomes[1] = 5;

    action_capture(game);

    ASSERT_EQ(game->incomes[1], 4);
}

TEST_F(game_fixture, action_capture_when_capture_unoccupied_city) {
    game->x = 2;
    game->y = 3;
    game->territory[3][2] = null_player;
    game->incomes[game->turn] = 5;

    action_capture(game);

    ASSERT_EQ(game->incomes[game->turn], 6);
}

TEST_F(game_fixture, action_move_returns_true_when_start_capturing) {
    insert_unit({.health = health_max, .x = 2, .y = 3, .enabled = true});
    units_select_at(&game->units, 2, 3);
    game->x = 5;
    game->y = 7;
    game->labels[7][5] = accessible_bit;
    game->dirty_labels = true;
    game->map[7][5] = tile_city;
    game->territory[7][5] = null_player;

    ASSERT_TRUE(action_move(game));

    const struct unit* const unit = units_const_get_at(&game->units, 5, 7);
    ASSERT_TRUE(unit);
    ASSERT_EQ(game->territory[7][5], null_player);
    ASSERT_FALSE(units_has_selection(&game->units));
}

TEST_F(game_fixture, action_move_returns_true_when_finish_capturing) {
    insert_unit({.health = health_max,
                 .x = 2,
                 .y = 3,
                 .enabled = true,
                 .capture_progress = health_max});
    units_select_at(&game->units, 2, 3);
    game->x = 2;
    game->y = 3;
    game->labels[3][2] = accessible_bit;
    game->dirty_labels = true;
    game->map[3][2] = tile_city;
    game->territory[3][2] = null_player;

    ASSERT_TRUE(action_move(game));

    const struct unit* const unit = units_const_get_at(&game->units, 2, 3);
    ASSERT_TRUE(unit);
    ASSERT_EQ(game->territory[3][2], unit->player);
    ASSERT_FALSE(units_has_selection(&game->units));
}

TEST_F(game_fixture, action_move_turns_false_when_cannot_move) {
    ASSERT_FALSE(action_move(game));
}

TEST_F(game_fixture, action_self_destruct_returns_true_when_selected) {
    insert_unit({.x = 2, .y = 3});
    units_select_at(&game->units, 2, 3);
    game->dirty_labels = true;

    ASSERT_TRUE(action_self_destruct(game));

    ASSERT_FALSE(game->dirty_labels);
}

TEST_F(game_fixture, action_self_destruct_returns_false) {
    ASSERT_FALSE(action_self_destruct(game));
}

TEST_F(game_fixture, at_least_two_alive_players_returns_true) {
    ++game->incomes[2];
    ++game->incomes[3];

    ASSERT_TRUE(at_least_two_alive_players(game));
}

TEST_F(game_fixture, at_least_two_alive_players_returns_false) {
    ASSERT_FALSE(at_least_two_alive_players(game));
}

TEST_F(game_fixture, action_surrender_returns_true_when_two_players_alive) {
    ++game->incomes[0];
    ++game->incomes[1];

    ASSERT_TRUE(action_surrender(game));

    ASSERT_EQ(game->incomes[0], 0);
    ASSERT_EQ(game->turn, 1);
}

TEST_F(game_fixture, action_surrender_returns_false_when_no_players_alive) {
    ASSERT_FALSE(action_surrender(game));
}

TEST_F(game_fixture, action_select_returns_true_when_unit_selectable) {
    insert_unit({.x = 2, .y = 3, .enabled = true});
    game->x = 2;
    game->y = 3;
    game->labels[7][5] = accessible_bit;
    game->dirty_labels = true;

    ASSERT_TRUE(action_select(game));

    ASSERT_EQ(game->labels[7][5], 0);
    ASSERT_TRUE(game->dirty_labels);
    const struct unit* const unit = units_const_get_at(&game->units, 2, 3);
    ASSERT_TRUE(unit);
}

TEST_F(game_fixture, action_select_returns_false_when_unit_unselectable) {
    ASSERT_FALSE(action_select(game));
}

TEST_F(game_fixture, action_highlight_returns_true_when_unit_selectable) {
    insert_unit({.x = 2, .y = 3});
    game->x = 2;
    game->y = 3;

    ASSERT_TRUE(action_highlight(game));

    ASSERT_EQ(game->labels[3][2], accessible_bit);
}

TEST_F(game_fixture, action_highlight_returns_false_when_unit_unselectable) {
    ASSERT_FALSE(action_highlight(game));
}