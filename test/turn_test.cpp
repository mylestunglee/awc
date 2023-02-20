#define EXPOSE_TURN_INTERNALS
#include "../src/bitarray.h"
#include "../src/constants.h"
#include "../src/turn.h"
#include "game_fixture.hpp"
#include "test_constants.hpp"

TEST_F(game_fixture, repair_units_increases_unit_health) {
    insert_unit({.x = 2, .y = 3});
    game->territory[3][2] = 0;

    repair_units(game);

    ASSERT_EQ(units_const_get_first(&game->units, game->turn)->health,
              HEAL_RATE);
    ASSERT_EQ(game->monies[0], -MONEY_SCALE * HEAL_RATE / HEALTH_MAX);
}

TEST_F(game_fixture, repair_units_ignores_unit_not_on_territory) {
    insert_unit();

    repair_units(game);

    ASSERT_EQ(game->monies[0], 0);
}

TEST_F(game_fixture, repair_units_caps_at_maximum_health) {
    insert_unit({.x = 2, .y = 3, .health = HEALTH_MAX - 1});
    game->territory[3][2] = 0;

    repair_units(game);

    ASSERT_EQ(units_const_get_first(&game->units, game->turn)->health,
              HEALTH_MAX);
    ASSERT_EQ(game->monies[0], -MONEY_SCALE / HEALTH_MAX);
}

TEST_F(game_fixture, start_turn) {
    insert_unit({.x = 2, .y = 3});
    game->territory[3][2] = 0;
    game->incomes[0] = MONEY_SCALE;

    start_turn(game);

    ASSERT_TRUE(units_const_get_first(&game->units, game->turn)->enabled);
    ASSERT_EQ(game->monies[0], MONEY_SCALE - MONEY_SCALE * HEAL_RATE / HEALTH_MAX);
}

TEST_F(game_fixture, end_turn) {
    insert_selected_unit({.x = 2, .y = 3, .enabled = true});
    game->dirty_labels = true;

    end_turn(game);

    ASSERT_FALSE(units_has_selection(&game->units));
    ASSERT_FALSE(game->dirty_labels);
    ASSERT_FALSE(units_const_get_first(&game->units, game->turn)->enabled);
}

TEST_F(game_fixture, next_alive_turn_increments_turn) {
    game->turn = PLAYERS_CAPACITY - 1;
    game->incomes[0] = MONEY_SCALE;

    next_alive_turn(game);

    ASSERT_EQ(game->turn, 0);
}

TEST_F(game_fixture, next_alive_turn_skips_dead_player) {
    game->incomes[2] = MONEY_SCALE;

    next_alive_turn(game);

    ASSERT_EQ(game->turn, 2);
}

TEST_F(game_fixture, exists_alive_non_bot_returns_true_when_alive_player) {
    game->incomes[0] = MONEY_SCALE;

    ASSERT_TRUE(exists_alive_non_bot(game));
}

TEST_F(game_fixture, exists_alive_non_bot_returns_false_when_alive_bot) {
    game->incomes[0] = MONEY_SCALE;
    bitarray_set(game->bots, game->turn);

    ASSERT_FALSE(exists_alive_non_bot(game));
}

TEST_F(game_fixture, exists_alive_non_bot_returns_false_when_dead_player) {
    ASSERT_FALSE(exists_alive_non_bot(game));
}

TEST_F(game_fixture, turn_next_sets_units_enabled) {
    insert_unit({.x = 2, .enabled = true});
    insert_unit({.x = 3, .player = 1, .enabled = false});

    turn_next(game);

    ASSERT_FALSE(units_const_get_first(&game->units, 0)->enabled);
    ASSERT_TRUE(units_const_get_first(&game->units, 1)->enabled);
    ASSERT_EQ(game->turn, 1);
}

TEST_F(game_fixture, turn_next_plays_bot_turn) {
    insert_unit({.x = 2, .player = 1, .enabled = true});
    bitarray_set(game->bots, 1);
    game->map[0][2] = TILE_PLAINS;
    game->map[0][3] = TILE_CITY;
    game->territory[0][3] = 2;
    game->incomes[2] = MONEY_SCALE;

    turn_next(game);

    ASSERT_EQ(units_const_get_first(&game->units, 1)->x, 3);
    ASSERT_EQ(game->turn, 2);
}
