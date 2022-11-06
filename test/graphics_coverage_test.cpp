#define EXPOSE_GRAPHICS_INTERNALS
#include "../constants.h"
#include "../graphics.h"
#include "game_fixture.hpp"
#include "test_constants.hpp"

TEST(graphics_coverage_test, apply_style) {
    apply_style('\x11', '\x00');
    apply_style('\x88', '\x00');
}

TEST_F(game_fixture, apply_pixel) {
    wchar_t symbol;
    uint8_t expected_style;
    render_pixel(game, 0, 0, 0, 0, false, false, &symbol, &expected_style);

    auto actual_style = apply_pixel(game, 0, 0, 0, 0, false, false, '\x00');

    ASSERT_EQ(actual_style, expected_style);
}

TEST(graphics_coverage_test, reset_cursor) { reset_cursor(); }

TEST(graphics_coverage_test, reset_black) { reset_black(); }

TEST(graphics_coverage_test, reset_style) { reset_style(); }

TEST_F(game_fixture, print_normal_text) {
    game->territory[0][0] = 0;
    insert_unit();

    print_normal_text(game);
}

TEST_F(game_fixture, print_attack_text) {
    insert_selected_unit();
    insert_unit({.x = 2, .y = 3});
    game->x = 2;
    game->y = 3;

    print_attack_text(game);
}

TEST_F(game_fixture, print_build_text) {
    game->map[0][0] = TILE_FACTORY;
    game->golds[0] = GOLD_SCALE;

    print_build_text(game);
}

TEST_F(game_fixture, print_text_when_attack_enabled) {
    insert_selected_unit();
    insert_unit({.x = 2, .y = 3});
    game->x = 2;
    game->y = 3;

    print_text(game, true, false);
}

TEST_F(game_fixture, print_text_when_build_enabled) {
    game->map[0][0] = TILE_FACTORY;

    print_text(game, false, true);
}

TEST_F(game_fixture, print_text_default) { print_text(game, false, false); }

TEST_F(game_fixture, graphics_render) { graphics_render(game); }