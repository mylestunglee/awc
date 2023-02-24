#define EXPOSE_GRAPHICS_INTERNALS
#include "../src/constants.h"
#include "../src/graphics.h"
#include "../src/unit_constants.h"
#include "game_fixture.hpp"
#include "test_constants.hpp"
#include "units_fixture.hpp"

TEST(graphics_test, graphics_init_sets_empty_character_locale) {
    using namespace std::literals::string_literals;

    graphics_initialise();

    std::string locale = setlocale(LC_CTYPE, nullptr);
    ASSERT_NE(locale, "C"s);

    // Reset to default locale
    setlocale(LC_CTYPE, "C");
}

TEST(graphics_test, render_block_with_no_progress_is_empty) {
    wchar_t symbol = 0;
    uint8_t style = '\x00';
    render_block(0, 2, &symbol, &style);
    ASSERT_EQ(symbol, ' ');
    ASSERT_EQ(style, '\x90');
}

TEST(graphics_test, render_block_with_near_completion_is_nearly_full) {
    wchar_t symbol = 0;
    uint8_t style = '\x00';
    render_block(99, 2, &symbol, &style);
    ASSERT_EQ(symbol, ' ');
    ASSERT_EQ(style, '\x0A');
}

TEST(graphics_test, render_block_with_half_completion_is_half_full) {
    wchar_t symbol = 0;
    uint8_t style = '\x00';
    render_block(50, 4, &symbol, &style);
    ASSERT_EQ(symbol, L'▌');
    ASSERT_EQ(style, '\xb0');
}

TEST(graphics_test, render_percentage_with_low_progress_is_right_aligned) {
    wchar_t left_symbol = ' ';
    wchar_t right_symbol = ' ';
    render_percentage(23, 5, &left_symbol);
    render_percentage(23, 6, &right_symbol);
    ASSERT_EQ(left_symbol, '2');
    ASSERT_EQ(right_symbol, '3');
}

TEST(graphics_test, render_percentage_with_high_progress_is_left_aligned) {
    wchar_t left_symbol = ' ';
    wchar_t right_symbol = ' ';
    render_percentage(57, 2, &left_symbol);
    render_percentage(57, 3, &right_symbol);
    ASSERT_EQ(left_symbol, '5');
    ASSERT_EQ(right_symbol, '7');
}

TEST(graphics_test, render_bar_overlaps_percentage_over_block) {
    wchar_t symbol = 0;
    uint8_t style = '\x00';
    render_bar(57, 2, &symbol, &style);
    ASSERT_EQ(symbol, '5');
    ASSERT_EQ(style, '\x0B');
}

TEST_F(units_fixture,
       render_unit_health_bar_returns_false_when_no_unit_exists) {
    wchar_t symbol = 0;
    uint8_t style = '\x00';

    ASSERT_FALSE(render_unit_health_bar(units, 0, 0, 2, 3, &symbol, &style));
}

TEST_F(units_fixture, render_unit_health_bar_returns_false_outside_health_bar) {
    wchar_t symbol = 0;
    uint8_t style = '\x00';

    ASSERT_FALSE(render_unit_health_bar(units, 0, 0, 0, 3, &symbol, &style));
}

TEST_F(units_fixture, render_unit_health_bar_returns_false_when_max_health) {
    insert({.health = HEALTH_MAX});
    wchar_t symbol = 0;
    uint8_t style = '\x00';

    ASSERT_FALSE(render_unit_health_bar(units, 0, 0, 2, 3, &symbol, &style));
}

TEST_F(units_fixture, render_unit_health_bar_shows_unit_health) {
    insert({.health = HEALTH_MAX - 1});
    wchar_t symbol = 0;
    uint8_t style = '\x00';

    ASSERT_TRUE(render_unit_health_bar(units, 0, 0, 2, 3, &symbol, &style));

    ASSERT_NE(symbol, 0);
    ASSERT_NE(style, '\x00');
}

TEST_F(units_fixture,
       render_capture_progress_bar_returns_false_when_no_unit_exists) {
    wchar_t symbol = 0;
    uint8_t style = '\x00';

    ASSERT_FALSE(
        render_capture_progress_bar(units, 0, 1, 2, 0, &symbol, &style));
}

TEST_F(units_fixture,
       render_capture_progress_bar_returns_false_when_not_capturing) {
    insert({});
    wchar_t symbol = 0;
    uint8_t style = '\x00';

    ASSERT_FALSE(
        render_capture_progress_bar(units, 0, 1, 2, 0, &symbol, &style));
}

TEST_F(units_fixture, render_capture_progress_bar_shows_capture_progress) {
    insert({.capture_progress = 1});
    wchar_t symbol = 0;
    uint8_t style = '\x00';

    ASSERT_TRUE(
        render_capture_progress_bar(units, 0, 1, 2, 0, &symbol, &style));

    ASSERT_NE(symbol, 0);
    ASSERT_NE(style, '\x00');
}

TEST_F(game_fixture, calc_tile_style_gets_terrian_style) {
    game->map[3][2] = TILE_PLAINS;

    ASSERT_EQ(calc_tile_style(game, 2, 3), '\xa2');
}

TEST_F(game_fixture, calc_tile_style_gets_building_style) {
    game->map[3][2] = TILE_CITY;
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
    game->map[3][2] = TILE_PLAINS;

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

TEST(graphics_test, calc_selection_symbol_shows_different_border_symbols) {
    wchar_t symbol = 0;
    std::set<wchar_t> symbols;

    for (auto tile_x = 0; tile_x < 8; ++tile_x)
        for (auto tile_y = 0; tile_y < 4; ++tile_y) {
            calc_selection_symbol(tile_x, tile_y, &symbol);
            symbols.insert(symbol);
        }

    ASSERT_EQ(symbols.size(), 6);
}

TEST_F(game_fixture, render_selection_ignores_nonselected_tile) {
    game->x = 2;
    game->y = 3;
    wchar_t symbol = 0;
    uint8_t style = '\x00';

    ASSERT_FALSE(
        render_selection(game, 3, 3, 0, 0, false, false, &symbol, &style));
}

TEST_F(game_fixture, render_selection_shows_selection) {
    game->x = 2;
    game->y = 3;
    wchar_t symbol = 0;
    uint8_t style = '\x00';

    ASSERT_TRUE(
        render_selection(game, 2, 3, 0, 0, false, false, &symbol, &style));

    ASSERT_NE(symbol, 0);
    ASSERT_NE(style, '\x00');
}

TEST(graphics_test, decode_texture_returns_true_on_transparent_texture) {
    wchar_t symbol = 0;
    uint8_t style = '\x00';

    ASSERT_TRUE(decode_texture('\x00', false, 0, &symbol, &style));
}

TEST(graphics_test, decode_texture_fetches_null_player_symbol) {
    wchar_t symbol = 0;
    uint8_t style = '\x00';

    ASSERT_FALSE(decode_texture('\xf0', false, NULL_PLAYER, &symbol, &style));

    ASSERT_EQ(symbol, ' ');
}

TEST(graphics_test, decode_texture_sets_player_symbol_and_style) {
    wchar_t symbol = 0;
    uint8_t style = '\x00';

    ASSERT_FALSE(decode_texture('\xf0', false, 2, &symbol, &style));

    ASSERT_EQ(symbol, '3');
    ASSERT_EQ(style, '\xf3');
}

TEST(graphics_test, decode_texture_fetches_texture) {
    wchar_t symbol = 0;
    uint8_t style = '\x00';

    ASSERT_FALSE(decode_texture('\x20', false, 0, &symbol, &style));

    ASSERT_EQ(symbol, '_');
}

TEST_F(units_fixture, render_unit_ignores_out_of_bounds) {
    insert({});
    wchar_t symbol = 0;
    uint8_t style = '\x00';

    ASSERT_FALSE(render_unit(units, 0, 0, 0, 0, &symbol, &style));
}

TEST_F(units_fixture, render_unit_ignores_no_unit) {
    wchar_t symbol = 0;
    uint8_t style = '\x00';

    ASSERT_FALSE(render_unit(units, 0, 0, 3, 1, &symbol, &style));

    ASSERT_EQ(symbol, 0);
    ASSERT_EQ(style, 0);
}

TEST_F(units_fixture, render_unit_ignores_transparent) {
    insert({.enabled = true});
    wchar_t symbol = 0;
    uint8_t style = '\x00';

    ASSERT_FALSE(render_unit(units, 0, 0, 2, 1, &symbol, &style));
}

TEST_F(units_fixture, render_unit_gives_unit_texture) {
    insert({.enabled = true});
    wchar_t symbol = 0;
    uint8_t style = '\x00';

    ASSERT_TRUE(render_unit(units, 0, 0, 3, 1, &symbol, &style));

    ASSERT_EQ(symbol, 'o');
    ASSERT_EQ(style, '\xf4');
}

TEST_F(units_fixture, render_unit_gives_shaded_unit_texture_when_disabled) {
    insert({});
    wchar_t symbol = 0;
    uint8_t style = '\x00';

    ASSERT_TRUE(render_unit(units, 0, 0, 3, 1, &symbol, &style));

    ASSERT_EQ(symbol, 'o');
    ASSERT_EQ(style, '\x04');
}

TEST(graphics_test, render_highlight_with_no_label_shows_no_highlight) {
    wchar_t symbol = 0;
    uint8_t style = '\x00';

    render_highlight('\x00', &symbol, &style);

    ASSERT_EQ(symbol, 0);
    ASSERT_EQ(style, '\x00');
}

TEST(graphics_test, render_highlight_shows_overlapping_accessible_style) {
    wchar_t symbol = 0;
    uint8_t style = '\x23';

    render_highlight(ACCESSIBLE_BIT, &symbol, &style);

    ASSERT_EQ(symbol, L'░');
    ASSERT_EQ(style, '\xe3');
}

TEST(graphics_test, render_highlight_shows_attackable_style) {
    wchar_t symbol = 0;
    uint8_t style = '\x00';

    render_highlight(ATTACKABLE_BIT, &symbol, &style);

    ASSERT_EQ(style, '\x90');
}

TEST(graphics_test, render_highlight_shows_accessible_and_attackable_style) {
    wchar_t symbol = 0;
    uint8_t style = '\x00';

    render_highlight(ACCESSIBLE_BIT | ATTACKABLE_BIT, &symbol, &style);

    ASSERT_EQ(style, '\xd0');
}

TEST_F(game_fixture, render_attack_arrows_show_arrows_on_even_pixel) {
    game->x = 5;
    game->y = 3;
    game->prev_x = 6;
    game->prev_y = 3;
    wchar_t symbol = 0;
    uint8_t style = '\x12';

    render_attack_arrows(game, 2, &symbol, &style);

    ASSERT_NE(symbol, 0);
    ASSERT_EQ(style, '\x92');
}

TEST_F(game_fixture, render_attack_arrows_pads_odd_pixel) {
    wchar_t symbol = 0;
    uint8_t style = '\x00';

    render_attack_arrows(game, 3, &symbol, &style);

    ASSERT_EQ(symbol, ' ');
}

TEST_F(game_fixture, render_attack_arrows_show_different_arrows) {
    game->x = 1;
    game->y = 1;
    std::set<wchar_t> symbols;

    for (auto [prev_x, prev_y] : std::vector<std::pair<grid_t, grid_t>>{
             {1, 2}, {1, 0}, {2, 1}, {0, 1}}) {
        game->prev_x = prev_x;
        game->prev_y = prev_y;
        wchar_t symbol = 0;
        uint8_t style = '\x00';
        render_attack_arrows(game, 0, &symbol, &style);
        symbols.insert(symbol);
    }

    ASSERT_EQ(symbols.size(), 4);
}

TEST_F(game_fixture, render_tile_shows_terrian) {
    wchar_t symbol = 0;
    uint8_t style = '\x00';
    game->map[3][2] = TILE_PLAINS;

    ASSERT_TRUE(render_tile(game, 2, 3, 0, 0, false, &symbol, &style));

    ASSERT_EQ(symbol, '"');
    ASSERT_EQ(style, '\xa2');
}

TEST_F(game_fixture, render_tile_shows_unhighlightable_building) {
    wchar_t symbol = 0;
    uint8_t style = '\x00';
    game->map[3][2] = TILE_CITY;

    ASSERT_TRUE(render_tile(game, 2, 3, 0, 2, false, &symbol, &style));

    ASSERT_EQ(symbol, '[');
    ASSERT_EQ(style, '\xf8');
}

TEST_F(game_fixture, render_tile_shows_highlightable_building) {
    wchar_t symbol = 0;
    uint8_t style = '\x00';
    game->map[3][2] = TILE_CITY;

    ASSERT_TRUE(render_tile(game, 2, 3, 0, 0, false, &symbol, &style));

    ASSERT_EQ(symbol, ' ');
    ASSERT_EQ(style, '\xf8');
}

TEST_F(game_fixture, render_tile_shows_arrows) {
    insert_selected_unit();
    wchar_t expected_symbol = 0;
    uint8_t expected_style = '\x00';
    wchar_t actual_symbol = 0;
    uint8_t actual_style = '\x00';
    game->x = 1;

    render_attack_arrows(game, 0, &expected_symbol, &expected_style);

    ASSERT_TRUE(
        render_tile(game, 0, 0, 0, 0, true, &actual_symbol, &actual_style));

    ASSERT_EQ(actual_symbol, expected_symbol);
    ASSERT_EQ(actual_style, expected_style);
}

namespace {
std::pair<wchar_t, uint8_t> render_pixel_helper(const struct game* const game,
                                                const grid_t x, const grid_t y,
                                                const grid_t tile_x,
                                                const grid_t tile_y) {
    wchar_t symbol = 0;
    uint8_t style = '\x00';
    auto rendered =
        render_pixel(game, x, y, tile_x, tile_y, game_is_attackable(game),
                     game_is_buildable(game), &symbol, &style);
    assert(rendered);
    return {symbol, style};
}
} // namespace

TEST_F(game_fixture, render_pixel_shows_unit_health_bar) {
    insert_unit({.health = HEALTH_MAX - 1});

    const auto [symbol, style] = render_pixel_helper(game, 0, 0, 2, 3);

    ASSERT_EQ(symbol, '9');
    ASSERT_EQ(style, '\x0a');
}

TEST_F(game_fixture, render_pixel_shows_capture_progress) {
    insert_unit({.capture_progress = CAPTURE_COMPLETION - 1});

    const auto [symbol, style] = render_pixel_helper(game, 0, 1, 2, 0);

    ASSERT_EQ(symbol, '9');
    ASSERT_EQ(style, '\x0a');
}

TEST_F(game_fixture, render_pixel_shows_selection) {
    const auto [symbol, style] = render_pixel_helper(game, 0, 0, 0, 0);

    ASSERT_EQ(symbol, L'┌');
    ASSERT_EQ(style, '\xe0');
}

TEST_F(game_fixture, render_pixel_shows_unit) {
    insert_unit({.enabled = true});

    const auto [symbol, style] = render_pixel_helper(game, 0, 0, 3, 1);

    ASSERT_EQ(symbol, 'o');
    ASSERT_EQ(style, '\xf4');
}

TEST_F(game_fixture, render_pixel_shows_tile) {
    game->map[3][2] = TILE_PLAINS;

    const auto [symbol, style] = render_pixel_helper(game, 2, 3, 0, 0);

    ASSERT_EQ(symbol, '"');
    ASSERT_EQ(style, '\xa2');
}
