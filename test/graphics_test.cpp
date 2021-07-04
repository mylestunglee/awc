#define expose_graphics_internals
#include "../graphics.h"
#include "game_fixture.hpp"

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
    ASSERT_EQ(style, '\xF4');
}

TEST_F(game_fixture, render_unit_gives_player_icon) {
    insert_unit({.player = 2, .enabled = true});
    wchar_t symbol = 0;
    uint8_t style = 0;
    ASSERT_TRUE(render_unit(game, 0, 0, 3, 1, &symbol, &style));
    ASSERT_EQ(symbol, '3');
    ASSERT_EQ(style, '\xF3');
}

TEST_F(game_fixture, render_unit_gives_shaded_unit_texture_when_disabled) {
    insert_unit({});
    wchar_t symbol = 0;
    uint8_t style = 0;
    ASSERT_TRUE(render_unit(game, 0, 0, 2, 1, &symbol, &style));
    ASSERT_EQ(symbol, 'o');
    ASSERT_EQ(style, '\x04');
}