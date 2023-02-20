#define EXPOSE_PARSE_INTERNALS
#include "../src/constants.h"
#include "../src/parse.h"
#include "game_fixture.hpp"
#include "test_constants.hpp"
#include <filesystem>

TEST(parse_test, parse_quit_returns_true) { ASSERT_TRUE(parse_quit(nullptr)); }

TEST(parse_test, parse_nothing_returns_false) {
    ASSERT_FALSE(parse_nothing(nullptr));
}

TEST_F(game_fixture, parse_bulid_returns_false_and_is_unsuccessful) {
    ASSERT_FALSE(parse_build(game, 0));

    ASSERT_EQ(game->units.size, 0);
}

TEST_F(game_fixture, parse_bulid_returns_false_and_is_successful) {
    game->monies[0] = MONEY_SCALE;
    game->x = 2;
    game->y = 3;
    game->territory[3][2] = 0;
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

TEST_F(game_fixture, parse_next_turn_returns_false) {
    insert_unit({.x = 2, .player = 1, .enabled = false});

    ASSERT_FALSE(parse_next_turn(game));

    ASSERT_EQ(game->turn, 1);
    ASSERT_EQ(game->x, 2);
}

TEST_F(game_fixture, parse_pan_up_returns_false) {
    game->x = 2;
    game->y = 3;

    ASSERT_FALSE(parse_pan_up(game));

    ASSERT_EQ(game->x, 2);
    ASSERT_EQ(game->y, 2);
    ASSERT_EQ(game->prev_x, 2);
    ASSERT_EQ(game->prev_y, 3);
}

TEST_F(game_fixture, parse_pan_left_returns_false) {
    game->x = 2;
    game->y = 3;

    ASSERT_FALSE(parse_pan_left(game));

    ASSERT_EQ(game->x, 1);
    ASSERT_EQ(game->y, 3);
    ASSERT_EQ(game->prev_x, 2);
    ASSERT_EQ(game->prev_y, 3);
}

TEST_F(game_fixture, parse_pan_down_returns_false) {
    game->x = 2;
    game->y = 3;

    ASSERT_FALSE(parse_pan_down(game));

    ASSERT_EQ(game->x, 2);
    ASSERT_EQ(game->y, 4);
    ASSERT_EQ(game->prev_x, 2);
    ASSERT_EQ(game->prev_y, 3);
}

TEST_F(game_fixture, parse_pan_right_returns_false) {
    game->x = 2;
    game->y = 3;

    ASSERT_FALSE(parse_pan_right(game));

    ASSERT_EQ(game->x, 3);
    ASSERT_EQ(game->y, 3);
    ASSERT_EQ(game->prev_x, 2);
    ASSERT_EQ(game->prev_y, 3);
}

TEST_F(game_fixture, parse_action_select_returns_false) {
    insert_unit({.enabled = true});

    ASSERT_FALSE(parse_action(game));

    ASSERT_TRUE(units_const_get_at(&game->units, 0, 0)->enabled);
}

TEST_F(game_fixture, parse_action_deselect_returns_false) {
    insert_selected_unit({.enabled = true});
    game->x = 1;

    ASSERT_FALSE(parse_action(game));

    ASSERT_FALSE(units_has_selection(&game->units));
}

TEST(parse_test, parse_command_returns_true) {
    parse_initialise();

    ASSERT_TRUE(parse_command(nullptr, '\0'));
}
