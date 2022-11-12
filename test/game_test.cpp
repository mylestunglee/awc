#define EXPOSE_GAME_INTERNALS
#include "../bitarray.h"
#include "../constants.h"
#include "../game.h"
#include "game_fixture.hpp"
#include "test_constants.hpp"
#include <fstream>

TEST_F(game_fixture, are_turns_empty_returns_false_when_unit_exists) {
    insert_unit();

    ASSERT_FALSE(are_turns_empty(game));
}

TEST_F(game_fixture, are_turns_empty_returns_false_when_gold_exists) {
    game->golds[game->turn] = GOLD_SCALE;

    ASSERT_FALSE(are_turns_empty(game));
}

TEST_F(game_fixture, are_turns_empty_returns_true_when_turns_are_empty) {
    ASSERT_TRUE(are_turns_empty(game));
}

TEST_F(game_fixture, skip_turns_adds_golds) {
    game->incomes[game->turn] = GOLD_SCALE;

    skip_turns(game);

    ASSERT_EQ(game->golds[game->turn], GOLD_SCALE);
}

TEST_F(game_fixture, skip_empty_turns_adds_golds) {
    game->incomes[game->turn] = GOLD_SCALE;

    skip_empty_turns(game);

    ASSERT_EQ(game->golds[game->turn], GOLD_SCALE);
}

TEST_F(game_fixture, game_load_resets_state) {
    game->map[3][2] = TILE_PLAINS;

    game_load(game, "");

    ASSERT_EQ(game->map[3][2], TILE_VOID);
}

TEST_F(game_fixture, game_load_sets_map) {
    using namespace std;
    const auto filename = "game1.txt";
    {
        ofstream file(filename);
        file << "map \"\nterritory 0 0 0";
    }

    ASSERT_FALSE(game_load(game, filename));

    ASSERT_EQ(game->map[0][0], TILE_PLAINS);
    ASSERT_EQ(game->territory[0][0], NULL_PLAYER);

    remove(filename);
}

TEST_F(game_fixture, game_load_computes_income) {
    using namespace std;
    const auto filename = "game2.txt";
    {
        ofstream file(filename);
        file << "map C\nterritory 0 0 0";
    }

    ASSERT_FALSE(game_load(game, filename));

    ASSERT_EQ(game->incomes[0], GOLD_SCALE);
    ASSERT_EQ(game->golds[0], GOLD_SCALE);

    remove(filename);
}

TEST_F(game_fixture, game_load_disables_unit) {
    using namespace std;
    const auto filename = "game3.txt";
    {
        ofstream file(filename);
        file << "map C\ninfantry 1 0 0";
    }

    ASSERT_FALSE(game_load(game, filename));

    ASSERT_FALSE(units_const_get_at(&game->units, 0, 0)->enabled);

    remove(filename);
}

TEST_F(game_fixture, game_save_returns_true_when_invalid_filename) {
    ASSERT_TRUE(game_save(nullptr, ""));
}

TEST_F(game_fixture, game_deselect_clears_selection_and_labels) {
    insert_selected_unit();
    game->dirty_labels = true;

    game_deselect(game);

    ASSERT_FALSE(units_has_selection(&game->units));
    ASSERT_FALSE(game->dirty_labels);
}

TEST_F(game_fixture, calc_damage_between_two_infantry) {
    insert_unit({.x = 0, .health = HEALTH_MAX});
    insert_unit({.x = 1, .health = HEALTH_MAX});
    game->map[0][1] = TILE_PLAINS;

    ASSERT_EQ(calc_damage(game, units_const_get_at(&game->units, 0, 0),
                          units_const_get_at(&game->units, 1, 0)),
              static_cast<health_t>(55.0 * 0.9));
}

TEST_F(game_fixture, calc_damage_pair_kill_attackee) {
    auto* attacker =
        insert_selected_unit({.x = 2, .y = 3, .health = HEALTH_MAX});
    auto* attackee = insert_unit({.x = 5, .y = 7, .health = 1});
    game->x = 5;
    game->y = 7;
    game->map[7][5] = TILE_PLAINS;

    health_t damage = 0;
    health_t counter_damage = 0;

    calc_damage_pair(game, attacker, attackee, &damage, &counter_damage);

    ASSERT_GT(damage, 0);
    ASSERT_EQ(counter_damage, 0);
}

TEST_F(game_fixture, calc_damage_pair_ranged_units_do_not_counter_attack) {
    const auto* attacker = insert_selected_unit(
        {.x = 2, .y = 3, .model = MODEL_ARTILLERY, .health = HEALTH_MAX});
    const auto* attackee = insert_unit({.x = 5, .y = 7, .health = HEALTH_MAX});
    game->x = 5;
    game->y = 7;
    game->map[7][5] = TILE_PLAINS;

    health_t damage = 0;
    health_t counter_damage = 0;

    calc_damage_pair(game, attacker, attackee, &damage, &counter_damage);

    ASSERT_GT(damage, 0);
    ASSERT_EQ(counter_damage, 0);
}

TEST_F(game_fixture, calc_damage_pair_when_counter_attacking) {
    auto* attacker =
        insert_selected_unit({.x = 2, .y = 3, .health = HEALTH_MAX});
    auto* attackee = insert_unit({.x = 5, .y = 7, .health = HEALTH_MAX});
    game->x = 5;
    game->y = 7;
    game->map[3][2] = TILE_PLAINS;
    game->map[7][5] = TILE_PLAINS;

    health_t damage = 0;
    health_t counter_damage = 0;

    calc_damage_pair(game, attacker, attackee, &damage, &counter_damage);

    ASSERT_EQ(damage, calc_damage(game, units_const_get_at(&game->units, 2, 3),
                                  units_const_get_at(&game->units, 5, 7)));
    ASSERT_EQ(counter_damage,
              ((HEALTH_MAX - damage) * damage + HEALTH_MAX / 2) / HEALTH_MAX);
}

TEST_F(game_fixture,
       calc_damage_pair_with_health_scales_with_calc_damage_pair) {
    auto* attacker =
        insert_selected_unit({.x = 2, .y = 3, .health = HEALTH_MAX});
    auto* attackee = insert_unit({.x = 5, .y = 7});
    game->x = 5;
    game->y = 7;

    health_t damage = 0;
    health_t counter_damage = 0;
    calc_damage_pair(game, attacker, attackee, &damage, &counter_damage);

    health_t health_half_damage = 0;
    calc_damage_pair_with_health(game, attacker, attackee, HEALTH_MAX / 2,
                                 &health_half_damage, &counter_damage);

    ASSERT_EQ(damage / 2, health_half_damage);
}

TEST_F(game_fixture, game_calc_damage_when_ranged) {
    auto* attacker = insert_selected_unit(
        {.x = 2, .y = 3, .model = MODEL_ARTILLERY, .health = HEALTH_MAX});
    auto* attackee = insert_unit({.x = 5, .y = 7, .health = HEALTH_MAX});

    health_t expected_damage;
    health_t expected_counter_damage;
    health_t actual_damage;
    health_t actual_counter_damage;

    game->x = 5;
    game->y = 7;

    calc_damage_pair(game, attacker, attackee, &expected_damage,
                     &expected_counter_damage);
    game_calc_damage(game, &actual_damage, &actual_counter_damage);

    ASSERT_EQ(actual_damage, expected_damage);
    ASSERT_EQ(actual_counter_damage, expected_counter_damage);
}

TEST_F(game_fixture, game_calc_damage_when_direct_and_no_merge) {
    auto* attacker =
        insert_selected_unit({.x = 2, .y = 3, .health = HEALTH_MAX});
    auto* attackee = insert_unit({.x = 5, .y = 7, .health = HEALTH_MAX});

    health_t expected_damage;
    health_t expected_counter_damage;
    health_t actual_damage;
    health_t actual_counter_damage;

    game->x = 5;
    game->y = 7;

    calc_damage_pair(game, attacker, attackee, &expected_damage,
                     &expected_counter_damage);
    game_calc_damage(game, &actual_damage, &actual_counter_damage);

    ASSERT_EQ(actual_damage, expected_damage);
    ASSERT_EQ(actual_counter_damage, expected_counter_damage);
}

TEST_F(game_fixture, game_calc_damage_when_direct_and_disabled_merge) {
    auto* attacker =
        insert_selected_unit({.x = 2, .y = 3, .health = HEALTH_MAX / 2});
    auto* attackee = insert_unit({.x = 5, .y = 7, .health = HEALTH_MAX});
    insert_unit({.x = 4, .y = 7, .health = HEALTH_MAX / 2});

    health_t expected_damage;
    health_t expected_counter_damage;
    health_t actual_damage;
    health_t actual_counter_damage;

    game->prev_x = 4;
    game->prev_y = 7;
    game->x = 5;
    game->y = 7;

    calc_damage_pair(game, attacker, attackee, &expected_damage,
                     &expected_counter_damage);
    game_calc_damage(game, &actual_damage, &actual_counter_damage);

    ASSERT_EQ(actual_damage, expected_damage);
    ASSERT_EQ(actual_counter_damage, expected_counter_damage);
}

TEST_F(game_fixture, game_calc_damage_when_merge) {
    auto* attacker =
        insert_selected_unit({.x = 2, .y = 3, .health = HEALTH_MAX / 2});
    auto* attackee = insert_unit({.x = 5, .y = 7, .health = HEALTH_MAX});
    insert_unit({.x = 4, .y = 7, .health = HEALTH_MAX / 2, .enabled = true});

    health_t expected_damage;
    health_t expected_counter_damage;
    health_t actual_damage;
    health_t actual_counter_damage;

    game->prev_x = 4;
    game->prev_y = 7;
    game->x = 5;
    game->y = 7;

    game_calc_damage(game, &actual_damage, &actual_counter_damage);
    attacker->health *= 2;
    calc_damage_pair(game, attacker, attackee, &expected_damage,
                     &expected_counter_damage);

    ASSERT_EQ(actual_damage, expected_damage);
    ASSERT_EQ(actual_counter_damage, expected_counter_damage);
}

TEST_F(game_fixture, game_is_attackable_returns_true_when_directly_attackable) {
    insert_selected_unit();
    game->prev_x = 2;
    game->x = 3;
    game->labels[0][2] = ACCESSIBLE_BIT;
    game->labels[0][3] = ATTACKABLE_BIT;

    ASSERT_TRUE(game_is_attackable(game));
}

TEST_F(game_fixture,
       game_is_attackable_returns_true_when_indirectly_attackable) {
    insert_selected_unit({.model = MODEL_ARTILLERY});
    game->x = 3;
    game->labels[0][3] = ATTACKABLE_BIT;

    ASSERT_TRUE(game_is_attackable(game));
}

TEST_F(game_fixture, game_is_attackable_returns_false_when_none_selected) {
    ASSERT_FALSE(game_is_attackable(game));
}

TEST_F(game_fixture, game_is_buildable_returns_true_when_buildable) {
    game->x = 2;
    game->y = 3;
    game->turn = 5;
    game->territory[3][2] = 5;
    game->map[3][2] = TILE_FACTORY;
    game->golds[5] = GOLD_SCALE;

    ASSERT_TRUE(game_is_buildable(game));
}

TEST_F(game_fixture, game_is_buildable_returns_false_when_unaffordable) {
    game->territory[3][2] = 5;
    game->map[3][2] = TILE_FACTORY;

    ASSERT_FALSE(game_is_buildable(game));
}

TEST_F(game_fixture, game_is_buildable_returns_false_at_void_tile) {
    ASSERT_FALSE(game_is_buildable(game));
}

TEST_F(game_fixture, game_is_alive_returns_true_when_player_has_unit) {
    insert_unit({});

    ASSERT_TRUE(game_is_alive(game, game->turn));
}

TEST_F(game_fixture, game_is_alive_returns_true_when_player_has_income) {
    game->incomes[game->turn] = GOLD_SCALE;

    ASSERT_TRUE(game_is_alive(game, game->turn));
}

TEST_F(game_fixture, game_is_alive_returns_false_when_player_has_nothing) {
    ASSERT_FALSE(game_is_alive(game, game->turn));
}

TEST_F(game_fixture, game_is_bot_returns_true_when_bot) {
    bitarray_set(game->bots, game->turn);

    ASSERT_TRUE(game_is_bot(game, game->turn));
}

TEST_F(game_fixture, game_is_bot_returns_false_when_not_bot) {
    ASSERT_FALSE(game_is_bot(game, game->turn));
}

TEST_F(game_fixture, game_is_friendly_returns_true_when_itself) {
    ASSERT_TRUE(game_is_friendly(game, 0));
}

TEST_F(game_fixture, game_is_friendly_returns_false_when_other) {
    ASSERT_FALSE(game_is_friendly(game, 1));
}

TEST_F(game_fixture, game_remove_player_makes_player_dead) {
    game->turn = 2;
    insert_unit({.player = game->turn});
    game->territory[5][3] = game->turn;
    game->incomes[game->turn] = GOLD_SCALE;

    game_remove_player(game, game->turn);

    ASSERT_FALSE(units_is_owner(&game->units, game->turn));
    ASSERT_EQ(game->territory[5][3], NULL_PLAYER);
    ASSERT_EQ(game->incomes[game->turn], 0);
}