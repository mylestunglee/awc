#include "../game.h"
#include "game_fixture.hpp"
#include <cstdio>
#include <fstream>

TEST_F(game_fixture, game_buildable_returns_true_when_buildable) {
    game->x = 2;
    game->y = 3;
    game->turn = 5;
    game->territory[3][2] = 5;
    game->map[3][2] = tile_factory;
    ASSERT_TRUE(game_buildable(game));
}

TEST_F(game_fixture, game_buildable_returns_false_when_unbuildable) {
    ASSERT_FALSE(game_buildable(game));
}

TEST_F(game_fixture, game_load_sets_map) {
    using namespace std;
    auto filename = "test_state.txt";
    {
        ofstream file(filename);
        file << "map \"";
    }
    game_load(game, filename);
    remove(filename);
    ASSERT_EQ(game->map[0][0], tile_plains);
}

// TODO: implement select next unit

TEST_F(game_fixture, game_attackable_returns_true_when_directly_attackable) {
    insert_unit({});
    units_select_at(&game->units, 0, 0);
    game->prev_x = 2;
    game->x = 3;
    game->labels[0][2] = accessible_bit;
    game->labels[0][3] = attackable_bit;
    ASSERT_TRUE(game_attackable(game));
}