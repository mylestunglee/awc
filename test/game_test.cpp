#define EXPOSE_GAME_INTERNALS
#include "../bitarray.h"
#include "../constants.h"
#include "../game.h"
#include "game_fixture.hpp"
#include "test_constants.hpp"
#include <cstdio>
#include <fstream>

TEST_F(game_fixture, game_load_sets_map) {
    using namespace std;
    auto filename = "test_state.txt";
    {
        ofstream file(filename);
        file << "map \"";
    }
    game_load(game, filename);
    remove(filename);
    ASSERT_EQ(game->map[0][0], TILE_PLAINS);
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

TEST_F(game_fixture, game_hover_next_unit_returns_true_when_hovering) {
    insert_unit({.x = 2, .enabled = true});

    ASSERT_TRUE(game_hover_next_unit(game));

    ASSERT_EQ(game->x, 2);
}

TEST_F(game_fixture, game_hover_next_unit_returns_false_when_not_hovering) {
    ASSERT_FALSE(game_hover_next_unit(game));
}

TEST_F(game_fixture, calc_damage_between_two_infantry) {
    insert_unit({.health = HEALTH_MAX, .model = MODEL_INFANTRY, .x = 0});
    insert_unit({.health = HEALTH_MAX, .model = MODEL_INFANTRY, .x = 1});
    game->map[0][1] = TILE_PLAINS;

    ASSERT_EQ(calc_damage(game, units_const_get_at(&game->units, 0, 0),
                          units_const_get_at(&game->units, 1, 0)),
              static_cast<health_t>(55.0 * 0.9 * 255.0 / 100.0));
}

TEST_F(game_fixture, game_simulate_attack_kill_attackee) {
    insert_selected_unit(
        {.health = HEALTH_MAX, .model = MODEL_INFANTRY, .x = 2, .y = 3});
    insert_unit({.health = 1, .model = MODEL_INFANTRY, .x = 5, .y = 7});
    game->x = 5;
    game->y = 7;
    game->map[7][5] = TILE_PLAINS;

    health_t damage = 0;
    health_t counter_damage = 0;

    game_simulate_attack(game, &damage, &counter_damage);

    ASSERT_GT(damage, 0);
    ASSERT_EQ(counter_damage, 0);
}

TEST_F(game_fixture, game_simulate_attack_ranged_units_do_not_counter_attack) {
    insert_selected_unit(
        {.health = HEALTH_MAX, .model = MODEL_ARTILLERY, .x = 2, .y = 3});
    insert_unit(
        {.health = HEALTH_MAX, .model = MODEL_INFANTRY, .x = 5, .y = 7});
    game->x = 5;
    game->y = 7;
    game->map[7][5] = TILE_PLAINS;

    health_t damage = 0;
    health_t counter_damage = 0;

    game_simulate_attack(game, &damage, &counter_damage);

    ASSERT_GT(damage, 0);
    ASSERT_EQ(counter_damage, 0);
}

TEST_F(game_fixture, game_simulate_attack_when_counter_attacking) {
    insert_selected_unit(
        {.health = HEALTH_MAX, .model = MODEL_INFANTRY, .x = 2, .y = 3});
    insert_unit(
        {.health = HEALTH_MAX, .model = MODEL_INFANTRY, .x = 5, .y = 7});
    game->x = 5;
    game->y = 7;
    game->map[3][2] = TILE_PLAINS;
    game->map[7][5] = TILE_PLAINS;

    health_t damage = 0;
    health_t counter_damage = 0;

    game_simulate_attack(game, &damage, &counter_damage);

    ASSERT_EQ(damage, calc_damage(game, units_const_get_at(&game->units, 2, 3),
                                  units_const_get_at(&game->units, 5, 7)));
    ASSERT_EQ(counter_damage, (HEALTH_MAX - damage) * damage / HEALTH_MAX);
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

    ASSERT_TRUE(game_is_buildable(game));
}

TEST_F(game_fixture, game_is_buildable_returns_false_at_void_tile) {
    ASSERT_FALSE(game_is_buildable(game));
}

TEST_F(game_fixture, game_is_alive_returns_true_when_player_has_unit) {
    insert_unit({});

    ASSERT_TRUE(game_is_alive(game, game->turn));
}

TEST_F(game_fixture, game_is_alive_returns_true_when_player_has_income) {
    game->incomes[game->turn] = 1000;

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
    game->incomes[game->turn] = 1000;

    game_remove_player(game, game->turn);

    ASSERT_FALSE(units_is_owner(&game->units, game->turn));
    ASSERT_EQ(game->territory[5][3], NULL_PLAYER);
    ASSERT_EQ(game->incomes[game->turn], 0);
}