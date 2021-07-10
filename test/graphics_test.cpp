#define expose_graphics_internals
#include "../graphics.h"
#include "game_fixture.hpp"
#include <gtest/gtest.h>

TEST(graphics_test, render_block_with_no_progress_is_empty) {
    wchar_t symbol = 0;
    uint8_t style = 0;
    render_block(0, 1, 1, &symbol, &style);
    ASSERT_EQ(symbol, ' ');
    ASSERT_EQ(style, '\x90');
}

TEST(graphics_test, render_block_with_near_completion_is_nearly_full) {
    wchar_t symbol = 0;
    uint8_t style = 0;
    render_block(254, 255, 1, &symbol, &style);
    ASSERT_EQ(symbol, ' ');
    ASSERT_EQ(style, '\x0A');
}

TEST(graphics_test, render_block_with_half_completion_is_half_full) {
    wchar_t symbol = 0;
    uint8_t style = 0;
    render_block(128, 255, 3, &symbol, &style);
    ASSERT_EQ(symbol, L'▌');
    ASSERT_EQ(style, '\xb0');
}

TEST(graphics_test, render_percentage_with_low_progress_is_right_aligned) {
    wchar_t left_symbol = ' ';
    wchar_t right_symbol = ' ';
    render_percentage(23, 100, 4, &left_symbol);
    render_percentage(23, 100, 5, &right_symbol);
    ASSERT_EQ(left_symbol, '2');
    ASSERT_EQ(right_symbol, '3');
}

TEST(graphics_test, render_percentage_with_high_progress_is_left_aligned) {
    wchar_t left_symbol = ' ';
    wchar_t right_symbol = ' ';
    render_percentage(57, 100, 1, &left_symbol);
    render_percentage(57, 100, 2, &right_symbol);
    ASSERT_EQ(left_symbol, '5');
    ASSERT_EQ(right_symbol, '7');
}

TEST(graphics_test, render_bar_overlaps_percentage_over_block) {
    wchar_t symbol = 0;
    uint8_t style = 0;
    render_bar(57, 100, 1, &symbol, &style);
    ASSERT_EQ(symbol, '5');
    ASSERT_EQ(style, '\x0B');
}

TEST_F(game_fixture, render_unit_health_bar_returns_false_when_no_unit_exists) {
    wchar_t symbol = 0;
    uint8_t style = 0;
    ASSERT_FALSE(render_unit_health_bar(game, 0, 0, 1, 3, &symbol, &style));
}

TEST_F(game_fixture, render_unit_health_bar_returns_false_when_max_health) {
    insert_unit({.health = health_max});
    wchar_t symbol = 0;
    uint8_t style = 0;
    ASSERT_FALSE(render_unit_health_bar(game, 0, 0, 1, 3, &symbol, &style));
}

TEST_F(game_fixture, render_unit_health_bar_shows_unit_health) {
    insert_unit({.health = health_max - 1});
    wchar_t symbol = 0;
    uint8_t style = 0;
    ASSERT_TRUE(render_unit_health_bar(game, 0, 0, 1, 3, &symbol, &style));
    ASSERT_NE(symbol, 0);
    ASSERT_NE(style, 0);
}

TEST_F(game_fixture,
       render_capture_progress_bar_returns_false_when_no_unit_exists) {
    wchar_t symbol = 0;
    uint8_t style = 0;
    ASSERT_FALSE(
        render_capture_progress_bar(game, 0, 1, 1, 0, &symbol, &style));
}

TEST_F(game_fixture,
       render_capture_progress_bar_returns_false_when_not_capturing) {
    insert_unit({});
    wchar_t symbol = 0;
    uint8_t style = 0;
    ASSERT_FALSE(
        render_capture_progress_bar(game, 0, 1, 1, 0, &symbol, &style));
}

TEST_F(game_fixture, render_capture_progress_bar_shows_capture_progress) {
    insert_unit({.capture_progress = 1});
    wchar_t symbol = 0;
    uint8_t style = 0;
    ASSERT_TRUE(render_capture_progress_bar(game, 0, 1, 1, 0, &symbol, &style));
    ASSERT_NE(symbol, 0);
    ASSERT_NE(style, 0);
}

TEST_F(game_fixture, calc_tile_style_gets_terrian_style) {
    game->map[3][2] = tile_plains;
    ASSERT_EQ(calc_tile_style(game, 2, 3), '\xa2');
}

TEST_F(game_fixture, calc_tile_style_gets_capturable_style) {
    game->map[3][2] = tile_city;
    game->territory[3][2] = 1;
    ASSERT_EQ(calc_tile_style(game, 2, 3), '\xf1');
}

TEST(graphics_test, calc_action_style_gets_attackable_style) {
    ASSERT_EQ(calc_action_style(true, false), '\x90');
}

TEST(graphics_test, calc_action_style_gets_buildable_style) {
    ASSERT_EQ(calc_action_style(false, true), '\xf0');
}

TEST(graphics_test, calc_action_style_gets_accessible_style) {
    ASSERT_EQ(calc_action_style(false, false), '\xe0');
}

TEST_F(game_fixture, calc_selection_style_overlaps_action_style_over_tile) {
    game->map[3][2] = tile_plains;
    ASSERT_EQ(calc_selection_style(game, 2, 3, false, false), '\xe2');
}

TEST(graphics_test, calc_selection_symbol_returns_false_when_not_in_border) {
    wchar_t symbol = 0;
    ASSERT_FALSE(calc_selection_symbol(1, 1, &symbol));
}

TEST(graphics_test, calc_selection_symbol_shows_border_symbol) {
    wchar_t symbol = 0;
    ASSERT_TRUE(calc_selection_symbol(0, 0, &symbol));    
    ASSERT_NE(symbol, 0);
}

TEST_F(game_fixture, render_selection_ignores_nonselected_tile) {
    game->x = 2;
    game->y = 3;
    wchar_t symbol = 0;
    uint8_t style = 0;
    ASSERT_FALSE(render_selection(game, 3, 3, 0, 0, false, false, &symbol, &style));
}

TEST_F(game_fixture, render_selection_shows_selection) {
    game->x = 2;
    game->y = 3;
    wchar_t symbol = 0;
    uint8_t style = 0;
    ASSERT_TRUE(render_selection(game, 2, 3, 0, 0, false, false, &symbol, &style));
    ASSERT_NE(symbol, 0);
    ASSERT_NE(style, 0);
}

// ------

TEST_F(game_fixture, render_unit_ignores_out_of_bounds) {
    insert_unit({});
    wchar_t symbol = 0;
    uint8_t style = 0;
    ASSERT_FALSE(render_unit(game, 0, 0, 0, 0, &symbol, &style));
}

TEST_F(game_fixture, render_unit_ignores_no_unit) {
    wchar_t symbol = 0;
    uint8_t style = 0;
    ASSERT_FALSE(render_unit(game, 0, 0, 2, 1, &symbol, &style));
    ASSERT_EQ(symbol, 0);
    ASSERT_EQ(style, 0);
}

TEST_F(game_fixture, render_unit_ignores_transparent) {
    insert_unit({.enabled = true});
    wchar_t symbol = 0;
    uint8_t style = 0;
    ASSERT_FALSE(render_unit(game, 0, 0, 1, 1, &symbol, &style));
}

TEST_F(game_fixture, render_unit_gives_unit_texture) {
    insert_unit({.enabled = true});
    wchar_t symbol = 0;
    uint8_t style = 0;
    ASSERT_TRUE(render_unit(game, 0, 0, 2, 1, &symbol, &style));
    ASSERT_EQ(symbol, 'o');
    ASSERT_EQ(style, '\xf4');
}

TEST_F(game_fixture, render_unit_gives_player_icon) {
    insert_unit({.player = 2, .enabled = true});
    wchar_t symbol = 0;
    uint8_t style = 0;
    ASSERT_TRUE(render_unit(game, 0, 0, 3, 1, &symbol, &style));
    ASSERT_EQ(symbol, '3');
    ASSERT_EQ(style, '\xf3');
}

TEST_F(game_fixture, render_unit_gives_shaded_unit_texture_when_disabled) {
    insert_unit({});
    wchar_t symbol = 0;
    uint8_t style = 0;
    ASSERT_TRUE(render_unit(game, 0, 0, 2, 1, &symbol, &style));
    ASSERT_EQ(symbol, 'o');
    ASSERT_EQ(style, '\x04');
}

TEST_F(game_fixture, render_selection_sets_correct_style_on_tile) {
    game->map[3][2] = tile_plains;
    game->x = 2;
    game->y = 3;
    wchar_t symbol = 0;
    uint8_t style = 0;
    ASSERT_TRUE(
        render_selection(game, 2, 3, 0, 0, false, false, &symbol, &style));
    ASSERT_NE(symbol, 0);
    ASSERT_EQ(style, '\xe2');
}