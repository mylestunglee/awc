#define EXPOSE_BOT_INTERNALS
#include "../src/bot.h"
#include "../src/constants.h"
#include "../src/unit_constants.h"
#include "game_fixture.hpp"
#include "test_constants.hpp"

TEST_F(game_fixture, simulate_attack_when_ranged) {
    insert_selected_unit({.model = MODEL_ARTILLERY, .health = HEALTH_MAX});
    insert_unit({.x = 2, .y = 3, .health = HEALTH_MAX});
    game->x = 2;
    game->y = 3;

    health_t damage, expected_counter_damage, actual_counter_damage;
    game_calc_damage(game, &damage, &expected_counter_damage);

    simulate_attack(game, &damage, &actual_counter_damage);

    ASSERT_EQ(actual_counter_damage, expected_counter_damage);
    ASSERT_EQ(game->prev_x, 0);
    ASSERT_EQ(game->prev_y, 0);
}

TEST_F(game_fixture, simulate_attack_sets_prev_position_when_direct) {
    insert_selected_unit({.health = HEALTH_MAX});
    insert_unit({.x = 2, .y = 3, .health = HEALTH_MAX});
    game->x = 2;
    game->y = 3;
    game->prev_x = 1;
    game->prev_y = 3;

    health_t damage, expected_counter_damage, actual_counter_damage;
    game_calc_damage(game, &damage, &expected_counter_damage);

    game->labels[3][1] = ACCESSIBLE_BIT;
    game->energies[3][1] = 1;
    game->prev_x = 0;
    game->prev_y = 0;

    simulate_attack(game, &damage, &actual_counter_damage);

    ASSERT_EQ(actual_counter_damage, expected_counter_damage);
    ASSERT_EQ(game->prev_x, 1);
    ASSERT_EQ(game->prev_y, 3);
}

TEST_F(game_fixture, find_attackee_maximises_metric) {
    insert_selected_unit();
    insert_unit({.x = 3, .health = HEALTH_MAX});
    const auto* const expected_attackee =
        insert_unit({.x = 5, .health = HEALTH_MAX / 2});
    game->labels[0][3] = ATTACKABLE_BIT;
    game->labels[0][5] = ATTACKABLE_BIT;

    auto actual_attackee = find_attackee(game, MODEL_INFANTRY);

    ASSERT_EQ(actual_attackee, expected_attackee);
}

TEST_F(game_fixture, handle_attack_reduces_attackee_health) {
    insert_selected_unit(
        {.model = MODEL_ARTILLERY, .health = HEALTH_MAX, .enabled = true});
    const auto* const attackee = insert_unit({.x = 2, .health = HEALTH_MAX});
    game->labels[0][2] = ATTACKABLE_BIT;
    game->dirty_labels = true;

    handle_attack(game, MODEL_ARTILLERY);

    ASSERT_LT(attackee->health, HEALTH_MAX);
}

TEST_F(game_fixture, update_max_energy_returns_updated_energy) {
    game->x = 2;
    game->y = 3;
    game->energies[3][2] = 7;
    game->labels[3][2] = ACCESSIBLE_BIT;
    grid_t updated_x = 0;
    grid_t updated_y = 0;

    const auto max_energy = update_max_energy(game, 5, &updated_x, &updated_y);

    ASSERT_EQ(max_energy, 7);
    ASSERT_EQ(updated_x, 2);
    ASSERT_EQ(updated_y, 3);
}

TEST_F(game_fixture, update_max_energy_returns_current_energy) {
    grid_t updated_x = 0;
    grid_t updated_y = 0;

    const auto max_energy = update_max_energy(game, 5, &updated_x, &updated_y);

    ASSERT_EQ(max_energy, 5);
    ASSERT_EQ(updated_x, 0);
    ASSERT_EQ(updated_y, 0);
}

TEST_F(game_fixture, find_nearest_capturable_returns_max_energy_when_found) {
    game->map[3][2] = TILE_CITY;
    game->territory[3][2] = 1;
    game->energies[3][2] = 5;
    game->labels[3][2] = ACCESSIBLE_BIT;
    game->map[3][4] = TILE_CITY;
    game->territory[3][4] = 1;
    game->energies[3][4] = 7;
    game->labels[3][4] = ACCESSIBLE_BIT;

    const auto max_energy = find_nearest_capturable(game);

    ASSERT_EQ(max_energy, 7);
    ASSERT_EQ(game->x, 4);
    ASSERT_EQ(game->y, 3);
}

TEST_F(game_fixture, find_nearest_capturable_returns_zero_when_unfound) {
    game->map[3][2] = TILE_CITY;
    game->territory[3][2] = 0;

    const auto max_energy = find_nearest_capturable(game);

    ASSERT_EQ(max_energy, 0);
}

TEST_F(game_fixture, handle_capture_when_uncapturable_unit) {
    const auto* const unit =
        insert_unit({.model = MODEL_ARTILLERY, .enabled = true});

    handle_capture(game, unit->model);

    ASSERT_TRUE(unit->enabled);
}

TEST_F(game_fixture, handle_capture_when_no_capturable_exists) {
    const auto* const unit = insert_unit({.enabled = true});

    handle_capture(game, unit->model);

    ASSERT_TRUE(unit->enabled);
}

TEST_F(game_fixture, handle_capture_when_capturable_exists) {
    game->map[3][2] = TILE_CITY;
    game->territory[3][2] = 1;
    game->energies[3][2] = 5;
    game->labels[3][2] = ACCESSIBLE_BIT;
    game->dirty_labels = true;

    const auto* const unit = insert_selected_unit({.enabled = true});

    handle_capture(game, unit->model);

    ASSERT_FALSE(unit->enabled);
    ASSERT_EQ(unit->x, 2);
    ASSERT_EQ(unit->y, 3);
}

TEST_F(game_fixture, handle_local_prioritises_attack_over_capture) {
    const auto* const attacker =
        insert_selected_unit({.x = 1, .health = HEALTH_MAX, .enabled = true});
    const auto* const attackee =
        insert_unit({.x = 3, .player = 1, .health = HEALTH_MAX});
    for (auto i = 1; i < 4; ++i)
        game->map[0][i] = TILE_PLAINS;
    game->map[0][0] = TILE_CITY;
    game->territory[0][0] = 1;

    handle_local(game, attacker);

    ASSERT_EQ(attacker->x, 2);
    ASSERT_LT(attackee->health, HEALTH_MAX);
    ASSERT_FALSE(game->dirty_labels);
}

TEST_F(game_fixture, handle_local_capture_when_unattackable) {
    const auto* const unit =
        insert_selected_unit({.health = HEALTH_MAX, .enabled = true});
    game->map[0][0] = TILE_PLAINS;
    game->map[0][1] = TILE_CITY;
    game->territory[0][1] = 1;

    handle_local(game, unit);

    ASSERT_EQ(unit->x, 1);
    ASSERT_EQ(unit->capture_progress, HEALTH_MAX);
    ASSERT_FALSE(game->dirty_labels);
}

TEST_F(game_fixture, handle_local_clears_labels_when_unactionable) {
    const auto* const unit = insert_selected_unit({.enabled = true});
    handle_local(game, unit);

    ASSERT_FALSE(game->dirty_labels);
}

TEST_F(game_fixture,
       find_nearest_attackable_attackee_ranged_returns_accessible_target) {
    grid_t nearest_x = 0;
    grid_t nearest_y = 0;
    const auto* const attackee = insert_unit({.x = 2, .y = 3});
    game->labels[3][0] = ACCESSIBLE_BIT;
    game->energies[3][0] = 5;

    const auto max_energy = find_nearest_attackable_attackee_ranged(
        game, MODEL_ARTILLERY, attackee, 0, &nearest_x, &nearest_y);

    ASSERT_EQ(max_energy, 5);
    ASSERT_EQ(nearest_x, 0);
    ASSERT_EQ(nearest_y, 3);
}

TEST_F(game_fixture,
       find_nearest_attackable_attackee_direct_returns_accessible_target) {
    grid_t nearest_x = 0;
    grid_t nearest_y = 0;
    const auto* const attackee = insert_unit({.x = 2, .y = 3});
    game->labels[3][1] = ACCESSIBLE_BIT;
    game->energies[3][1] = 5;

    const auto max_energy = find_nearest_attackable_attackee_direct(
        game, attackee, 0, &nearest_x, &nearest_y);

    ASSERT_EQ(max_energy, 5);
    ASSERT_EQ(nearest_x, 1);
    ASSERT_EQ(nearest_y, 3);
}

TEST_F(game_fixture, find_nearest_attackable_attackee_when_ranged) {
    grid_t dummy;
    const auto* const attackee = insert_unit({.x = 2, .y = 3});
    game->labels[3][0] = ACCESSIBLE_BIT;
    game->energies[3][0] = 5;

    const auto max_energy = find_nearest_attackable_attackee(
        game, MODEL_ARTILLERY, attackee, 0, &dummy, &dummy);

    ASSERT_EQ(max_energy, 5);
}

TEST_F(game_fixture, find_nearest_attackable_attackee_when_direct) {
    grid_t dummy;
    const auto* const attackee = insert_unit({.x = 2, .y = 3});
    game->labels[3][1] = ACCESSIBLE_BIT;
    game->energies[3][1] = 5;

    const auto max_energy = find_nearest_attackable_attackee(
        game, MODEL_INFANTRY, attackee, 0, &dummy, &dummy);

    ASSERT_EQ(max_energy, 5);
}

TEST_F(game_fixture, find_nearest_attackable) {
    grid_t dummy;
    insert_unit({.x = 2, .y = 3, .player = 1});
    game->labels[3][1] = ACCESSIBLE_BIT;
    game->energies[3][1] = 5;

    const auto max_energy =
        find_nearest_attackable(game, MODEL_INFANTRY, &dummy, &dummy);

    ASSERT_EQ(max_energy, 5);
}

TEST_F(game_fixture, find_nearest_target_when_attackable_target) {
    grid_t nearest_x = 0;
    grid_t nearest_y = 0;
    insert_unit({.x = 2, .y = 3, .player = 1});
    game->labels[3][1] = ACCESSIBLE_BIT;
    game->energies[3][1] = 5;

    ASSERT_TRUE(
        find_nearest_target(game, MODEL_INFANTRY, &nearest_x, &nearest_y));

    ASSERT_EQ(nearest_x, 1);
    ASSERT_EQ(nearest_y, 3);
}

TEST_F(game_fixture, find_nearest_target_when_capturable_target) {
    grid_t nearest_x = 0;
    grid_t nearest_y = 0;
    game->map[3][2] = TILE_CITY;
    game->territory[3][2] = 1;
    game->energies[3][2] = 5;
    game->labels[3][2] = ACCESSIBLE_BIT;

    ASSERT_TRUE(
        find_nearest_target(game, MODEL_INFANTRY, &nearest_x, &nearest_y));

    ASSERT_EQ(nearest_x, 2);
    ASSERT_EQ(nearest_y, 3);
}

TEST_F(game_fixture,
       find_nearest_target_favours_capturable_over_attackable_target) {
    grid_t nearest_x = 0;
    grid_t nearest_y = 0;
    insert_unit({.x = 2, .y = 3, .player = 1});
    game->labels[3][1] = ACCESSIBLE_BIT;
    game->energies[3][1] = 5;
    game->map[5][2] = TILE_CITY;
    game->territory[5][2] = 1;
    game->energies[5][2] = 5;
    game->labels[5][2] = ACCESSIBLE_BIT;

    ASSERT_TRUE(
        find_nearest_target(game, MODEL_INFANTRY, &nearest_x, &nearest_y));

    ASSERT_EQ(nearest_x, 2);
    ASSERT_EQ(nearest_y, 5);
}

TEST_F(game_fixture, find_nearest_target_when_no_target) {
    grid_t dummy;

    ASSERT_FALSE(find_nearest_target(game, MODEL_HELICOPTER, &dummy, &dummy));
}

TEST_F(game_fixture, move_towards_target_moves_one_turn) {
    for (auto x = 0; x < 5; ++x) {
        game->labels[0][x] = ACCESSIBLE_BIT;
        game->energies[0][x] = 5 - x;
    }

    move_towards_target(game, MODEL_INFANTRY, 5, 0);

    ASSERT_EQ(game->x, 3);
}

TEST_F(game_fixture, handle_nonlocal_moves_unit_when_target_exists) {
    auto* unit = insert_selected_unit({.enabled = true});

    for (auto i = 1; i < 5; ++i) {
        game->labels[0][i] = ACCESSIBLE_BIT;
        game->map[0][i] = TILE_PLAINS;
    }
    game->labels[0][5] = ACCESSIBLE_BIT;
    game->map[0][5] = TILE_CITY;
    game->territory[0][5] = NULL_PLAYER;
    game->dirty_labels = true;

    handle_nonlocal(game, unit);

    ASSERT_EQ(unit->x, 3);
    ASSERT_FALSE(game->dirty_labels);
}

TEST_F(game_fixture, handle_nonlocal_does_nothing_when_no_target_exists) {
    auto* unit = insert_selected_unit();
    game->dirty_labels = true;

    handle_nonlocal(game, unit);

    ASSERT_FALSE(game->dirty_labels);
}

TEST_F(game_fixture, interact_unit_when_local_move_is_possible) {
    auto* unit = insert_unit({.enabled = true});

    game->map[0][1] = TILE_CITY;
    game->territory[0][1] = NULL_PLAYER;
    game->labels[0][1] = ACCESSIBLE_BIT;
    game->dirty_labels = true;

    interact_unit(game, unit);

    ASSERT_FALSE(unit->enabled);
}

TEST_F(game_fixture, interact_unit_when_disabled_unit) {
    auto* unit = insert_unit();

    game->map[0][1] = TILE_CITY;
    game->territory[0][1] = NULL_PLAYER;
    game->labels[0][1] = ACCESSIBLE_BIT;
    game->dirty_labels = true;

    interact_unit(game, unit);

    ASSERT_EQ(unit->x, 0);
}

TEST_F(game_fixture, interact_unit_when_nonlocal_move_is_possible) {
    auto* unit = insert_unit({.enabled = true});

    for (auto i = 1; i < 5; ++i) {
        game->labels[0][i] = ACCESSIBLE_BIT;
        game->map[0][i] = TILE_PLAINS;
    }
    game->labels[0][5] = ACCESSIBLE_BIT;
    game->map[0][5] = TILE_CITY;
    game->territory[0][5] = NULL_PLAYER;
    game->dirty_labels = true;

    interact_unit(game, unit);

    ASSERT_FALSE(unit->enabled);
}

TEST_F(game_fixture, interact_unit_maintains_enabled_when_move_is_impossible) {
    auto* unit = insert_unit({.enabled = true});

    interact_unit(game, unit);

    ASSERT_TRUE(unit->enabled);
}

TEST_F(game_fixture, interact_units) {
    const auto* unit = insert_unit({.enabled = true});

    game->map[0][1] = TILE_CITY;
    game->territory[0][1] = NULL_PLAYER;
    game->labels[0][1] = ACCESSIBLE_BIT;
    game->dirty_labels = true;

    interact_units(game);

    ASSERT_FALSE(unit->enabled);
}

TEST_F(game_fixture, bot_play_clears_labels) {
    game->dirty_labels = true;

    bot_play(game);

    ASSERT_FALSE(game->dirty_labels);
}

TEST_F(game_fixture, bot_play_interacts_units) {
    const auto* unit = insert_unit({.enabled = true});

    game->map[0][1] = TILE_CITY;
    game->territory[0][1] = NULL_PLAYER;
    game->labels[0][1] = ACCESSIBLE_BIT;
    game->dirty_labels = true;

    bot_play(game);

    ASSERT_FALSE(unit->enabled);
}

TEST_F(game_fixture, bot_play_build_units) {
    game->map[0][0] = TILE_FACTORY;
    game->territory[0][0] = 0;
    game->golds[0] = GOLD_SCALE;

    bot_play(game);

    ASSERT_EQ(game->units.size, 1);
}
