#define expose_turn_internals
#include "../bitarray.h"
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

TEST_F(game_fixture, start_turn) {
    insert_unit({.x = 2, .y = 3});
    game->territory[3][2] = game->turn;
    game->incomes[game->turn] = 1;
    start_turn(game);
    ASSERT_TRUE(units_const_get_first(&game->units, game->turn)->enabled);
    ASSERT_EQ(game->golds[game->turn],
              gold_scale - gold_scale * heal_rate / health_max);
}

TEST_F(game_fixture, end_turn) {
    insert_unit({.x = 2, .y = 3, .enabled = true});
    units_select_at(&game->units, 2, 3);
    game->dirty_labels = true;
    end_turn(game);
    ASSERT_FALSE(units_has_selection(&game->units));
    ASSERT_FALSE(game->dirty_labels);
    ASSERT_FALSE(units_const_get_first(&game->units, game->turn)->enabled);
}

TEST_F(game_fixture, next_alive_turn_increments_turn) {
    game->turn = players_capacity - 1;
    game->incomes[0] = 1;
    next_alive_turn(game);
    ASSERT_EQ(game->turn, 0);
}

TEST_F(game_fixture, next_alive_turn_skips_dead_player) {
    game->incomes[2] = 1;
    next_alive_turn(game);
    ASSERT_EQ(game->turn, 2);
}

TEST_F(game_fixture, exists_alive_non_bot_returns_true_when_alive_player) {
    game->incomes[game->turn] = 1;
    ASSERT_TRUE(exists_alive_non_bot(game));
}

TEST_F(game_fixture, exists_alive_non_bot_returns_false_when_alive_bot) {
    game->incomes[game->turn] = 1;
    bitarray_set(game->bots, game->turn);
    ASSERT_FALSE(exists_alive_non_bot(game));
}

TEST_F(game_fixture, exists_alive_non_bot_returns_false_when_dead_player) {
    ASSERT_FALSE(exists_alive_non_bot(game));
}

TEST_F(game_fixture, turn_next_sets_units_enabled) {
    insert_unit({.player = 0, .x = 2, .enabled = true});
    insert_unit({.player = 1, .x = 3, .enabled = false});
    game->turn = 0;
    turn_next(game);
    ASSERT_FALSE(units_const_get_first(&game->units, 0)->enabled);
    ASSERT_TRUE(units_const_get_first(&game->units, 1)->enabled);
    ASSERT_EQ(game->turn, 1);
    ASSERT_EQ(game->x, 3);
}

TEST_F(game_fixture, turn_next_plays_bot_turn) {
    insert_unit({.player = 1, .x = 2, .enabled = true});
    bitarray_set(game->bots, 1);
    game->map[0][2] = tile_plains;
    game->map[0][3] = tile_city;
    game->territory[0][3] = 2;
    game->incomes[2] = 1;
    turn_next(game);
    ASSERT_EQ(units_const_get_first(&game->units, 1)->x, 3);
    ASSERT_EQ(game->turn, 2);
}
