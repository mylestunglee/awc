#define EXPOSE_PARSE_INTERNALS
#include "../constants.h"
#include "../parse.h"
#include "game_fixture.hpp"
#include "test_constants.hpp"
#include <filesystem>

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

void parse_save_load_n_returns_false(
    struct game* const game, const char* const filename,
    const std::function<bool(struct game* const)> parse_save,
    const std::function<bool(struct game* const)> parse_load) {
    assert(!std::filesystem::exists(filename));

    game->map[3][2] = TILE_PLAINS;

    ASSERT_FALSE(parse_save(game));

    game_initialise(game);

    ASSERT_FALSE(parse_load(game));

    ASSERT_EQ(game->map[3][2], TILE_PLAINS);

    remove(filename);
}

TEST_F(game_fixture, parse_save_load_1_returns_false) {
    parse_save_load_n_returns_false(game, "state1.txt", parse_save_1,
                                    parse_load_1);
}

TEST_F(game_fixture, parse_save_load_2_returns_false) {
    parse_save_load_n_returns_false(game, "state2.txt", parse_save_2,
                                    parse_load_2);
}

TEST_F(game_fixture, parse_save_load_3_returns_false) {
    parse_save_load_n_returns_false(game, "state1.txt", parse_save_3,
                                    parse_load_3);
}

TEST_F(game_fixture, parse_save_load_4_returns_false) {
    parse_save_load_n_returns_false(game, "state1.txt", parse_save_4,
                                    parse_load_4);
}

TEST_F(game_fixture, parse_save_load_5_returns_false) {
    parse_save_load_n_returns_false(game, "state1.txt", parse_save_5,
                                    parse_load_5);
}
