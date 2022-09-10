#define EXPOSE_PARSE_INTERNALS
#include "../parse.h"
#include "../constants.h"
#include "game_fixture.hpp"
#include "test_constants.hpp"

TEST(parse_test, parse_error_returns_true) {
    ASSERT_TRUE(parse_error(nullptr));
}

TEST(parse_test, parse_nothing_returns_false) {
    ASSERT_FALSE(parse_nothing(nullptr));
}

TEST_F(game_fixture, parse_bulid_returns_false_and_is_unsuccessful) {
    ASSERT_FALSE(parse_build(game, 0));

    ASSERT_EQ(game->units.size, 0);
}

TEST_F(game_fixture, parse_bulid_returns_false_and_is_successful) {
    game->golds[game->turn] = GOLD_SCALE;
    game->x = 2;
    game->y = 3;
    game->territory[3][2] = game->turn;
    game->map[3][2] = TILE_FACTORY;

    ASSERT_FALSE(parse_build(game, 0));

    ASSERT_EQ(game->units.size, 1);
}

TEST_F(game_fixture, parse_bulid_1_returns_false) {
    ASSERT_FALSE(parse_build_1(game));
}

TEST_F(game_fixture, parse_bulid_2_returns_false) {
    ASSERT_FALSE(parse_build_2(game));
}

TEST_F(game_fixture, parse_bulid_3_returns_false) {
    ASSERT_FALSE(parse_build_3(game));
}

TEST_F(game_fixture, parse_bulid_4_returns_false) {
    ASSERT_FALSE(parse_build_4(game));
}

TEST_F(game_fixture, parse_bulid_5_returns_false) {
    ASSERT_FALSE(parse_build_5(game));
}

TEST_F(game_fixture, parse_bulid_6_returns_false) {
    ASSERT_FALSE(parse_build_6(game));
}

TEST_F(game_fixture, parse_bulid_7_returns_false) {
    ASSERT_FALSE(parse_build_7(game));
}

TEST_F(game_fixture, parse_bulid_8_returns_false) {
    ASSERT_FALSE(parse_build_8(game));
}

TEST_F(game_fixture, parse_bulid_9_returns_false) {
    ASSERT_FALSE(parse_build_9(game));
}

TEST_F(game_fixture, parse_bulid_10_returns_false) {
    ASSERT_FALSE(parse_build_10(game));
}

