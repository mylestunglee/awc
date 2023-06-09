#define EXPOSE_ACTION_INTERNALS
#include "../src/action.h"
#include "../src/constants.h"
#include "game_fixture.hpp"
#include "test_constants.hpp"

TEST_F(game_fixture, move_selected_unit_when_no_merge) {
    const auto* const unit = insert_selected_unit(
        {.x = 2, .y = 3, .health = HEALTH_MAX, .enabled = true});

    ASSERT_EQ(move_selected_unit(game, 5, 3), HEALTH_MAX);

    ASSERT_EQ(unit->x, 5);
    ASSERT_FALSE(unit->enabled);
}

TEST_F(game_fixture, move_selected_unit_when_merge_with_disabled_unit) {
    insert_selected_unit({.x = 2, .y = 3, .health = 7, .enabled = true});
    insert_unit({.x = 5, .y = 3, .health = 11});

    ASSERT_EQ(move_selected_unit(game, 5, 3), 7);

    ASSERT_EQ(game->units.size, 1);
}

TEST_F(game_fixture, move_selected_unit_when_merge_with_enabled_unit) {
    insert_selected_unit({.x = 2, .y = 3, .health = 7, .enabled = true});
    insert_unit({.x = 5, .y = 3, .health = 11, .enabled = true});

    ASSERT_EQ(move_selected_unit(game, 5, 3), 18);

    ASSERT_EQ(game->units.size, 1);
}

TEST_F(game_fixture, action_attack_returns_true_when_unattackable) {
    ASSERT_TRUE(action_attack(game));
}

TEST_F(game_fixture,
       action_attack_returns_false_where_direct_unit_with_counter_damage) {
    const auto* const attacker = insert_selected_unit(
        {.x = 2, .y = 3, .health = HEALTH_MAX, .enabled = true});
    const auto* const attackee =
        insert_unit({.x = 5, .y = 7, .health = HEALTH_MAX});
    game->prev_x = 4;
    game->prev_y = 7;
    game->x = 5;
    game->y = 7;
    game->map[7][4] = TILE_PLAINS;
    game->map[7][5] = TILE_PLAINS;
    game->labels[7][4] = ACCESSIBLE_BIT;
    game->labels[7][5] = ATTACKABLE_BIT;
    game->dirty_labels = true;

    ASSERT_FALSE(action_attack(game));

    ASSERT_EQ(attacker->x, 4);
    ASSERT_EQ(attacker->y, 7);
    ASSERT_FALSE(attacker->enabled);
    ASSERT_LT(attacker->health, HEALTH_MAX);
    ASSERT_GT(attacker->health, attackee->health);
    ASSERT_FALSE(units_has_selection(&game->units));
}

TEST_F(game_fixture, action_attack_returns_false_where_ranged_unit_kills) {
    const auto* const attacker = insert_selected_unit({.x = 2,
                                                       .y = 3,
                                                       .model = MODEL_ARTILLERY,
                                                       .health = HEALTH_MAX,
                                                       .enabled = true});
    insert_unit({.x = 5, .y = 7, .health = 1});
    game->x = 5;
    game->y = 7;
    game->map[7][5] = TILE_PLAINS;
    game->labels[7][5] = ATTACKABLE_BIT;
    game->dirty_labels = true;

    ASSERT_FALSE(action_attack(game));

    ASSERT_FALSE(units_exists_at(&game->units, 5, 7));
    ASSERT_FALSE(units_has_selection(&game->units));
    ASSERT_FALSE(attacker->enabled);
}

TEST_F(game_fixture, action_attack_returns_false_where_counter_damage_kills) {
    insert_selected_unit({.x = 2, .y = 3, .health = 1, .enabled = true});
    insert_unit({.x = 5, .y = 7, .health = HEALTH_MAX});
    game->prev_x = 4;
    game->prev_y = 7;
    game->x = 5;
    game->y = 7;
    game->map[7][4] = TILE_PLAINS;
    game->map[7][5] = TILE_PLAINS;
    game->labels[7][4] = ACCESSIBLE_BIT;
    game->labels[7][5] = ATTACKABLE_BIT;
    game->dirty_labels = true;

    ASSERT_FALSE(action_attack(game));

    ASSERT_FALSE(units_exists_at(&game->units, 4, 7));
    ASSERT_FALSE(units_has_selection(&game->units));
}

TEST_F(game_fixture, action_build_returns_true_when_model_is_unbuildable) {
    game->monies[0] = MONEY_SCALE;
    game->x = 2;
    game->y = 3;
    game->territory[3][2] = 0;
    game->map[3][2] = TILE_CITY;

    ASSERT_TRUE(action_build(game, MODEL_INFANTRY));
}

TEST_F(game_fixture, action_build_returns_false_when_model_is_buildable) {
    game->monies[0] = MONEY_SCALE;
    game->x = 2;
    game->y = 3;
    game->territory[3][2] = 0;
    game->map[3][2] = TILE_FACTORY;

    ASSERT_FALSE(action_build(game, MODEL_INFANTRY));

    const struct unit* const unit = units_const_get_at(&game->units, 2, 3);
    ASSERT_TRUE(unit);
    ASSERT_EQ(unit->health, HEALTH_MAX);
    ASSERT_EQ(unit->model, MODEL_INFANTRY);
    ASSERT_EQ(unit->player, game->turn);
    ASSERT_EQ(unit->x, 2);
    ASSERT_EQ(unit->y, 3);
    ASSERT_FALSE(unit->enabled);
    ASSERT_EQ(unit->capture_progress, 0);
}

TEST_F(game_fixture, can_selected_unit_capture_returns_true_when_building) {
    insert_selected_unit({.x = 2, .y = 3});
    game->x = 2;
    game->y = 3;
    game->map[3][2] = TILE_CITY;
    game->territory[3][2] = 1;

    ASSERT_TRUE(can_selected_unit_capture(game));
}

TEST_F(game_fixture, can_selected_unit_capture_returns_false_when_unbuilding) {
    insert_selected_unit({.x = 2, .y = 3});
    game->x = 2;
    game->y = 3;

    ASSERT_FALSE(can_selected_unit_capture(game));
}

TEST_F(game_fixture, action_capture_when_capture_enemy_hq) {
    game->x = 2;
    game->y = 3;
    game->territory[3][2] = 1;
    game->map[3][2] = TILE_HQ;
    game->incomes[0] = 5000;
    game->incomes[1] = 5000;

    action_capture(game);

    ASSERT_EQ(game->incomes[1], 0);
    ASSERT_EQ(game->territory[3][2], game->turn);
    ASSERT_EQ(game->incomes[0], 10000);
}

TEST_F(game_fixture, action_capture_when_capture_enemy_city) {
    game->x = 2;
    game->y = 3;
    game->territory[3][2] = 1;
    game->incomes[1] = 5000;

    action_capture(game);

    ASSERT_EQ(game->incomes[1], 4000);
}

TEST_F(game_fixture, action_capture_when_capture_unoccupied_city) {
    game->x = 2;
    game->y = 3;
    game->territory[3][2] = NULL_PLAYER;
    game->incomes[0] = 5000;

    action_capture(game);

    ASSERT_EQ(game->incomes[0], 6000);
}

TEST_F(game_fixture, action_move_returns_false_when_start_capturing) {
    const auto* const unit = insert_selected_unit(
        {.x = 2, .y = 3, .health = HEALTH_MAX, .enabled = true});
    game->x = 5;
    game->y = 7;
    game->labels[7][5] = ACCESSIBLE_BIT;
    game->dirty_labels = true;
    game->map[7][5] = TILE_CITY;
    game->territory[7][5] = NULL_PLAYER;

    ASSERT_FALSE(action_move(game));

    ASSERT_EQ(unit->x, 5);
    ASSERT_EQ(unit->y, 7);
    ASSERT_EQ(game->territory[7][5], NULL_PLAYER);
    ASSERT_FALSE(units_has_selection(&game->units));
}

TEST_F(game_fixture, action_move_returns_false_when_finish_capturing) {
    insert_selected_unit({.x = 2,
                          .y = 3,
                          .health = HEALTH_MAX,
                          .capture_progress = HEALTH_MAX,
                          .enabled = true});
    game->x = 2;
    game->y = 3;
    game->labels[3][2] = ACCESSIBLE_BIT;
    game->dirty_labels = true;
    game->map[3][2] = TILE_CITY;
    game->territory[3][2] = NULL_PLAYER;

    ASSERT_FALSE(action_move(game));

    const struct unit* const unit = units_const_get_at(&game->units, 2, 3);
    ASSERT_TRUE(unit);
    ASSERT_EQ(game->territory[3][2], unit->player);
    ASSERT_FALSE(units_has_selection(&game->units));
}

TEST_F(game_fixture, action_move_turns_true_when_cannot_move) {
    ASSERT_TRUE(action_move(game));
}

TEST_F(game_fixture, action_self_destruct_returns_false_when_selected) {
    insert_selected_unit({.x = 2, .y = 3});
    game->dirty_labels = true;

    ASSERT_FALSE(action_self_destruct(game));

    ASSERT_FALSE(game->dirty_labels);
}

TEST_F(game_fixture, action_self_destruct_returns_true_when_error) {
    ASSERT_TRUE(action_self_destruct(game));
}

TEST_F(game_fixture, at_least_two_alive_players_returns_true) {
    ++game->incomes[2];
    ++game->incomes[3];

    ASSERT_TRUE(at_least_two_alive_players(game));
}

TEST_F(game_fixture, at_least_two_alive_players_returns_false) {
    ASSERT_FALSE(at_least_two_alive_players(game));
}

TEST_F(game_fixture, action_surrender_returns_false_when_two_players_alive) {
    ++game->incomes[0];
    ++game->incomes[1];

    ASSERT_FALSE(action_surrender(game));

    ASSERT_EQ(game->incomes[0], 0);
    ASSERT_EQ(game->turn, 1);
}

TEST_F(game_fixture, action_surrender_returns_true_when_no_players_alive) {
    ASSERT_TRUE(action_surrender(game));
}

TEST_F(game_fixture, action_select_returns_false_when_unit_selectable) {
    insert_unit({.x = 2, .y = 3, .enabled = true});
    game->x = 2;
    game->y = 3;
    game->labels[7][5] = ACCESSIBLE_BIT;
    game->dirty_labels = true;

    ASSERT_FALSE(action_select(game));

    ASSERT_EQ(game->labels[7][5], 0);
    ASSERT_TRUE(game->dirty_labels);
    const struct unit* const unit = units_const_get_at(&game->units, 2, 3);
    ASSERT_TRUE(unit);
}

TEST_F(game_fixture, action_select_returns_true_when_unit_unselectable) {
    ASSERT_TRUE(action_select(game));
}

TEST_F(game_fixture, action_highlight_returns_false_when_unit_selectable) {
    insert_unit({.x = 2, .y = 3});
    game->x = 2;
    game->y = 3;

    ASSERT_FALSE(action_highlight(game));

    ASSERT_EQ(game->labels[3][2], ACCESSIBLE_BIT);
}

TEST_F(game_fixture, action_highlight_returns_true_when_unit_unselectable) {
    ASSERT_TRUE(action_highlight(game));
}

TEST_F(game_fixture, find_next_unit_returns_first_enabled_unit_while_hovering) {
    insert_unit({.x = 2, .enabled = true});
    insert_unit({.x = 3, .enabled = false});
    insert_unit({.x = 5, .enabled = true});
    game->x = 5;

    auto unit = find_next_unit(game);

    ASSERT_TRUE(unit);
    ASSERT_EQ(unit->x, 2);
}

TEST_F(game_fixture, find_next_unit_returns_null_when_no_more_enabled_units) {
    insert_unit({.x = 2, .enabled = false});
    game->x = 2;

    auto unit = find_next_unit(game);

    ASSERT_FALSE(unit);
}

TEST_F(game_fixture,
       find_next_unit_returns_first_enabled_unit_while_not_hovering) {
    insert_unit({.x = 2, .enabled = false});
    insert_unit({.x = 3, .enabled = true});
    insert_unit({.x = 5, .enabled = false});

    auto unit = find_next_unit(game);

    ASSERT_TRUE(unit);
    ASSERT_EQ(unit->x, 3);
}

TEST_F(game_fixture, find_next_unit_returns_null_when_no_units) {
    ASSERT_FALSE(find_next_unit(game));
}

TEST_F(game_fixture, action_hover_next_unit_returns_false_when_hovering) {
    insert_unit({.x = 2, .enabled = true});

    ASSERT_FALSE(action_hover_next_unit(game));

    ASSERT_EQ(game->x, 2);
}

TEST_F(game_fixture, action_hover_next_unit_returns_true_when_not_hovering) {
    ASSERT_TRUE(action_hover_next_unit(game));
}
