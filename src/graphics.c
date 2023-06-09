#include "graphics.h"
#include "console.h"
#include "constants.h"
#include "format_constants.h"
#include "unit_constants.h"
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

#define TILE_WIDTH 8
#define TILE_HEIGHT 4
#define UNIT_LEFT 2
#define UNIT_TOP 1
#define UNIT_WIDTH 5
#define UNIT_HEIGHT 2

#define UNIT_RIGHT (UNIT_LEFT + UNIT_WIDTH - 1)
#define UNIT_BOTTOM (UNIT_TOP + UNIT_HEIGHT - 1)

#define ACCESSIBLE_STYLE '\xe0'
#define ATTACKABLE_STYLE '\x90'
#define ACCESSIBLE_ATTACKABLE_STYLE '\xd0'
#define BUILDABLE_STYLE '\xf0'
#define BLOCK_SYMBOL_COUNT 3

static const uint8_t unit_symbols[14] = {' ', '_',  'o', 'x', '<', '>', 'v',
                                         '^', '\\', '/', '[', ']', '-', '='};

static const uint8_t player_styles[PLAYERS_CAPACITY + 1] = {
    '\xf4', '\xf1', '\xf3', '\xf2', '\xf8'};

static const uint8_t tile_styles[TERRIAN_CAPACITY] = {
    '\x80', '\xa2', '\x32', '\x13', '\x3b',
    '\xc4', '\xd4', '\x4c', '\x78', '\x78'};
static const uint8_t
    unit_textures[MODEL_CAPACITY][UNIT_HEIGHT][(UNIT_WIDTH + 1) / 2] = {
        {{'\x03', '\xf3', '\x00'}, {'\x08', '\x88', '\x00'}},
        {{'\x0E', '\xfe', '\x00'}, {'\x08', '\x88', '\x00'}},
        {{'\x0A', '\xfd', '\x00'}, {'\x03', '\x33', '\x00'}},
        {{'\xab', '\xfc', '\xd0'}, {'\x3e', '\xee', '\x30'}},
        {{'\xb1', '\xfc', '\xe0'}, {'\x3e', '\xee', '\x30'}},
        {{'\x08', '\xf8', '\x00'}, {'\x03', '\xe3', '\x00'}},
        {{'\x88', '\xf8', '\x80'}, {'\x33', '\x33', '\x30'}},
        {{'\x0A', '\xfa', '\x00'}, {'\x03', '\xe3', '\x00'}},
        {{'\xaa', '\xfa', '\xa0'}, {'\x33', '\x33', '\x30'}},
        {{'\x0E', '\x4e', '\x10'}, {'\x0B', '\xfc', '\xe0'}},
        {{'\x91', '\x11', '\x10'}, {'\x59', '\xfc', '\xe0'}},
        {{'\x91', '\x11', '\x10'}, {'\x57', '\xf7', '\x60'}},
        {{'\x22', '\x22', '\x20'}, {'\x92', '\xfc', '\xe0'}},
        {{'\x2a', '\xfa', '\x20'}, {'\x92', '\x2c', '\xe0'}},
        {{'\x88', '\xf8', '\x80'}, {'\x92', '\x22', '\xa0'}}};

static const uint8_t building_textures[BUILDING_CAPACITY][TILE_HEIGHT]
                                      [(TILE_WIDTH + 1) / 2] = {
                                          {
                                              {'\x00', '\xb1', '\xfc', '\x00'},
                                              {'\x00', '\xb1', '\xb1', '\x1c'},
                                              {'\xb1', '\x1c', '\xb1', '\x1c'},
                                              {'\xb1', '\x1c', '\x00', '\x00'},
                                          },
                                          {
                                              {'\x00', '\x00', '\x00', '\x00'},
                                              {'\xb9', '\x2b', '\xf2', '\xac'},
                                              {'\xb1', '\x11', '\x11', '\x1c'},
                                              {'\xb1', '\x11', '\x11', '\x1c'},
                                          },
                                          {
                                              {'\x00', '\x00', '\x9f', '\xa0'},
                                              {'\x00', '\x00', '\xc1', '\xb0'},
                                              {'\x22', '\x22', '\xc1', '\xb2'},
                                              {'\xb1', '\x11', '\x11', '\x1c'},
                                          },
                                          {
                                              {'\x00', '\x00', '\x00', '\x00'},
                                              {'\x00', '\x00', '\xbc', '\xfc'},
                                              {'\x22', '\x22', '\xbc', '\x1c'},
                                              {'\xb1', '\x11', '\x11', '\x1c'},
                                          },
                                          {{'\x00', '\xbe', '\xfc', '\x00'},
                                           {'\x00', '\xb1', '\x1c', '\x00'},
                                           {'\xb1', '\x11', '\x11', '\x1c'},
                                           {'\xb1', '\x11', '\x11', '\x1c'}}};

static const char* const tile_names[TILE_CAPACITY] = {
    "void", "plains",  "forest",  "mountains", "beach",
    "sea",  "reef",    "river",   "road",      "bridge",
    "city", "factory", "airport", "habour",    "HQ"};

void graphics_initialise() { setlocale(LC_CTYPE, "C.UTF-8"); }

void render_block(const uint32_t percent, const grid_t tile_x,
                  wchar_t* const symbol, uint8_t* const style) {
    const uint8_t styles[] = {'\x90', '\xb0', '\xa0'};
    const uint8_t inverted_styles[] = {'\x09', '\x0B', '\x0A'};
    const uint8_t style_index = (BLOCK_SYMBOL_COUNT * percent) / 100;

    const wchar_t block_symbols[] = {L'[', L'|', L']'};
    const int8_t steps =
        (((BLOCK_SYMBOL_COUNT + 1) * UNIT_WIDTH + 1) * percent) / 100 -
        (BLOCK_SYMBOL_COUNT + 1) * (tile_x - UNIT_LEFT) - 1;
    if (steps < 0) {
        *style = styles[style_index];
        *symbol = ' ';
    } else if (steps < BLOCK_SYMBOL_COUNT) {
        *style = styles[style_index];
        *symbol = block_symbols[steps];
    } else {
        *style = inverted_styles[style_index];
        *symbol = ' ';
    }
}

void render_percentage(const uint32_t percent, const grid_t tile_x,
                       wchar_t* const symbol) {
    assert(percent < 100);
    const wchar_t left_digit = '0' + percent / 10;
    const wchar_t right_digit = '0' + percent % 10;
    if (percent >= 50) {
        if (tile_x == UNIT_LEFT) {
            assert(*symbol == ' ');
            *symbol = left_digit;
        } else if (tile_x == UNIT_LEFT + 1) {
            assert(*symbol == ' ');
            *symbol = right_digit;
        }
    } else {
        if (tile_x == UNIT_RIGHT - 1) {
            assert(*symbol == ' ');
            *symbol = left_digit;
        } else if (tile_x == UNIT_RIGHT) {
            assert(*symbol == ' ');
            *symbol = right_digit;
        }
    }
}

void render_bar(const uint32_t progress, const grid_t tile_x,
                wchar_t* const symbol, uint8_t* const style) {
    render_block(progress, tile_x, symbol, style);
    render_percentage(progress, tile_x, symbol);
}

bool render_unit_health_bar(const struct units* const units, const grid_t x,
                            const grid_t y, const grid_t tile_x,
                            const grid_t tile_y, wchar_t* const symbol,
                            uint8_t* const style) {

    // Display health bar on the bottom of unit
    if (tile_y != TILE_HEIGHT - 1)
        return false;

    if (tile_x < UNIT_LEFT || tile_x > UNIT_RIGHT)
        return false;

    const struct unit* const unit = units_const_get_at_safe(units, x, y);
    if (!unit)
        return false;

    const health_t health = unit->health;

    // Hide health bar on full-health units
    if (health == HEALTH_MAX)
        return false;

    render_bar(health, tile_x, symbol, style);

    return true;
}

bool render_capture_progress_bar(const struct units* const units,
                                 const grid_t x, const grid_t y,
                                 const grid_t tile_x, const grid_t tile_y,
                                 wchar_t* const symbol, uint8_t* const style) {

    // Display health bar on the bottom of building
    if (tile_y != 0)
        return false;

    if (tile_x < UNIT_LEFT || tile_x >= UNIT_LEFT + UNIT_WIDTH)
        return false;

    const struct unit* const unit =
        units_const_get_at_safe(units, x, y - (grid_t)1);
    if (!unit)
        return false;

    const capture_progress_t capture_progress = unit->capture_progress;

    // Hide health bar on full-health units
    if (capture_progress == 0)
        return false;

    render_bar(capture_progress / (CAPTURE_COMPLETION / HEALTH_MAX), tile_x,
               symbol, style);

    return true;
}

uint8_t calc_tile_style(const struct game* const game, const grid_t x,
                        const grid_t y) {
    const tile_t tile = game->map[y][x];
    return tile < TERRIAN_CAPACITY ? tile_styles[tile]
                                   : player_styles[game->territory[y][x]];
}

uint8_t calc_action_style(const bool attackable, const bool buildable) {
    if (attackable)
        return ATTACKABLE_STYLE;
    else if (buildable)
        return BUILDABLE_STYLE;
    else
        return ACCESSIBLE_STYLE;
}

uint8_t calc_selection_style(const struct game* const game, const grid_t x,
                             const grid_t y, const bool attackable,
                             const bool buildable) {
    return (calc_tile_style(game, x, y) & '\x0f') |
           calc_action_style(attackable, buildable);
}

bool calc_selection_symbol(const grid_t tile_x, const grid_t tile_y,
                           wchar_t* const symbol) {
    const bool top = tile_y == 0;
    const bool bottom = tile_y == TILE_HEIGHT - 1;
    const bool left = tile_x == 0;
    const bool right = tile_x == TILE_WIDTH - 1;

    if (!(top || bottom || left || right))
        return false;

    if (top && left)
        *symbol = L'┌';
    else if (top && right)
        *symbol = L'┐';
    else if (bottom && left)
        *symbol = L'└';
    else if (bottom && right)
        *symbol = L'┘';
    else if (top || bottom)
        *symbol = L'─';
    else if (left || right)
        *symbol = L'│';

    return true;
}

bool render_selection(const struct game* const game, const grid_t x,
                      const grid_t y, const grid_t tile_x, const grid_t tile_y,
                      const bool attackable, const bool buildable,
                      wchar_t* const symbol, uint8_t* const style) {
    if (game->x != x || game->y != y)
        return false;

    const bool selected = calc_selection_symbol(tile_x, tile_y, symbol);

    if (selected)
        *style = calc_selection_style(game, x, y, attackable, buildable);

    return selected;
}

// Returns true iff texture is transparent
bool decode_texture(const uint8_t textures, const bool polarity,
                    const player_t player, wchar_t* const symbol,
                    uint8_t* const style) {

    *style = player_styles[player];

    // Extract 4-bits corresponding to texture coordinate
    const uint8_t texture = polarity ? (textures & '\x0f') : (textures >> 4);

    // Handle transparent pixel
    if (texture == '\x00')
        return true;
    else if (texture == '\x0f') {
        if (player == NULL_PLAYER)
            *symbol = ' ';
        else
            *symbol = '1' + player;
    } else
        *symbol = unit_symbols[texture - 1];

    return false;
}

// Attempt to find symbol style pair from rendering coordinates
bool render_unit(const struct units* const units, const grid_t x,
                 const grid_t y, const grid_t tile_x, const grid_t tile_y,
                 wchar_t* const symbol, uint8_t* const style) {

    // Out of bounds
    if (UNIT_LEFT > tile_x || tile_x > UNIT_RIGHT || UNIT_TOP > tile_y ||
        tile_y > UNIT_BOTTOM)
        return false;

    const struct unit* const unit = units_const_get_at_safe(units, x, y);
    if (!unit)
        return false;

    const bool transparent = decode_texture(
        unit_textures[unit->model][tile_y - UNIT_TOP][(tile_x - UNIT_LEFT) / 2],
        (tile_x - UNIT_LEFT) % 2 != 0, unit->player, symbol, style);

    if (transparent)
        return false;

    // Dim forecolours if disabled
    if (!unit->enabled)
        *style &= '\x0F';

    return true;
}

void render_highlight(const uint8_t label, wchar_t* const symbol,
                      uint8_t* const style) {

    assert(!(label & ~(ACCESSIBLE_BIT | ATTACKABLE_BIT)));

    // Apply label hightlighting
    if (!label)
        return;

    // Clear foreground style
    *style &= '\x0f';
    *symbol = L'░';

    // Set foreground style
    switch (label) {
    case ACCESSIBLE_BIT:
        *style |= ACCESSIBLE_STYLE;
        break;
    case ATTACKABLE_BIT:
        *style |= ATTACKABLE_STYLE;
        break;
    case ACCESSIBLE_BIT | ATTACKABLE_BIT:
        *style |= ACCESSIBLE_ATTACKABLE_STYLE;
        break;
    }
}

void render_attack_arrows(const struct game* const game, const grid_t tile_x,
                          wchar_t* const symbol, uint8_t* const style) {

    if (tile_x % 2 != 0) {
        *symbol = ' ';
    } else {
        if ((grid_t)(game->prev_x + 1) == game->x) {
            assert(game->prev_y == game->y);
            *symbol = L'>';
        } else if ((grid_t)(game->prev_x - 1) == game->x) {
            assert(game->prev_y == game->y);
            *symbol = L'<';
        } else if ((grid_t)(game->prev_y + 1) == game->y) {
            assert(game->prev_x == game->x);
            *symbol = L'v';
        } else {
            assert((grid_t)(game->prev_y - 1) == game->y);
            assert(game->prev_x == game->x);
            *symbol = L'^';
        }

        // Set foreground colour to attackable style
        *style = (*style & '\x0f') | ATTACKABLE_STYLE;
    }
}

bool render_tile(const struct game* const game, const grid_t x, const grid_t y,
                 const grid_t tile_x, const grid_t tile_y,
                 const bool attackable, wchar_t* const symbol,
                 uint8_t* const style) {

    const tile_t tile = game->map[y][x];
    bool highlightable = true;

    if (tile < TERRIAN_CAPACITY) {
        *style = tile_styles[tile];
        *symbol = tile_symbols[tile];
    } else {
        // If texture is transparent, texture is highlightable
        highlightable = decode_texture(
            building_textures[tile - TERRIAN_CAPACITY][tile_y][tile_x / 2],
            tile_x % 2 != 0, game->territory[y][x], symbol, style);

        if (highlightable)
            *symbol = ' ';
    }

    // Show arrows highlighting position to attack unit
    if (attackable && x == game->prev_x && y == game->prev_y &&
        units_is_direct(units_const_get_selected(&game->units)->model))
        render_attack_arrows(game, tile_x, symbol, style);
    else if (highlightable)
        render_highlight(game->labels[y][x], symbol, style);

    return true;
}

bool render_pixel(const struct game* const game, const grid_t x, const grid_t y,
                  const grid_t tile_x, const grid_t tile_y,
                  const bool attackable, const bool buildable,
                  wchar_t* const symbol, uint8_t* const style) {
    return render_unit_health_bar(&game->units, x, y, tile_x, tile_y, symbol,
                                  style) ||
           render_capture_progress_bar(&game->units, x, y, tile_x, tile_y,
                                       symbol, style) ||
           render_selection(game, x, y, tile_x, tile_y, attackable, buildable,
                            symbol, style) ||
           render_unit(&game->units, x, y, tile_x, tile_y, symbol, style) ||
           render_tile(game, x, y, tile_x, tile_y, attackable, symbol, style);
}

void apply_style(const uint8_t style, const uint8_t prev_style) {
    if (style != prev_style) {
        const uint8_t forecolour = style >> 4;
        const uint8_t backcolour = style & 15;
        const uint8_t prev_forecolour = prev_style >> 4;
        const uint8_t prev_backcolour = prev_style & 15;
        bool carry = false;

        wprintf(L"%c[", '\x1b');
        if (forecolour != prev_forecolour) {
            carry = true;
            if (forecolour < 8)
                wprintf(L"%u", forecolour + 30);
            else
                wprintf(L"%u", forecolour + 82);
        }
        if (backcolour != prev_backcolour) {
            if (carry)
                wprintf(L";");
            if (backcolour < 8)
                wprintf(L"%u", backcolour + 40);
            else
                wprintf(L"%u", backcolour + 92);
        }
        wprintf(L"m");
    }
}

// Returns previous style
uint8_t apply_pixel(const struct game* const game, const grid_t x,
                    const grid_t y, const grid_t tile_x, const grid_t tile_y,
                    const bool attackable, const bool buildable,
                    const uint8_t prev_style) {
    wchar_t symbol;
    uint8_t style;
    const bool rendered = render_pixel(game, x, y, tile_x, tile_y, attackable,
                                       buildable, &symbol, &style);
    assert(rendered);
    apply_style(style, prev_style);
    wprintf(L"%lc", symbol);
    return style;
}

void reset_cursor(void) {
    wprintf(L"\033[0;0H");
    wprintf(L"\033[2J\033[1;1H");
}

void reset_black(void) { wprintf(L"%c[30;40m", '\x1b'); }

void reset_style(void) { wprintf(L"%c[0m", '\x1b'); }

void print_normal_text(const struct game* const game) {
    wprintf(L"turn=");
    for (player_t player = 0; player < PLAYERS_CAPACITY; ++player)
        if (game_is_alive(game, player)) {
            if (player == game->turn)
                wprintf(L"[" PLAYER_FORMAT "]", player + 1);
            else
                wprintf(L"" PLAYER_FORMAT, player + 1);
        }
    wprintf(L" money=" MONEY_FORMAT " tile=%s", game->monies[game->turn],
            tile_names[game->map[game->y][game->x]]);

    {
        const player_t territory = game->territory[game->y][game->x];
        if (territory != NULL_PLAYER)
            wprintf(L" territory=%u", territory + 1);
    }

    {
        const struct unit* unit =
            units_const_get_at_safe(&game->units, game->x, game->y);
        if (unit)
            wprintf(L" model=%s", model_names[unit->model]);
    }

    wprintf(L"\n");
}

void print_attack_text(const struct game* const game) {
    health_t damage, counter_damage;
    game_calc_damage(game, &damage, &counter_damage);
    wprintf(L"attack mode: damage=%u%% counter-damage=%u%%\n", damage,
            counter_damage);
}

void print_build_text(const struct game* const game) {
    const tile_t tile = game->map[game->y][game->x];
    assert(tile >= TERRIAN_CAPACITY);
    const tile_t building = tile - TERRIAN_CAPACITY;

    wprintf(L"build mode:");
    for (model_t model = building_buildable_models[building];
         model < building_buildable_models[building + 1]; ++model) {
        if (game->monies[game->turn] >= model_costs[model])
            wprintf(L" %s=" MODEL_FORMAT, model_names[model], model + 1);
    }
    wprintf(L"\n");
}

void print_text(const struct game* const game, const bool attackable,
                const bool buildable) {
    if (attackable)
        print_attack_text(game);
    else if (buildable)
        print_build_text(game);
    else
        print_normal_text(game);
}

void graphics_render(const struct game* const game) {
    const bool attackable = game_is_attackable(game);
    const bool buildable = game_is_buildable(game);
    assert(!(attackable && buildable));

    reset_cursor();

    int console_width, console_height;
    get_console_size(&console_width, &console_height);
    const grid_t screen_width = console_width / TILE_WIDTH;
    const grid_t screen_height = console_height / TILE_HEIGHT;
    const grid_t screen_left = game->x - screen_width / 2 + 1;
    const grid_t screen_right = game->x + screen_width / 2 + 1;
    const grid_t screen_top = game->y - screen_height / 2;
    const grid_t screen_bottom = game->y + screen_height / 2;

    for (grid_t y = screen_top; y != screen_bottom; ++y) {
        for (uint8_t tile_y = 0; tile_y < TILE_HEIGHT; ++tile_y) {
            reset_black();
            uint8_t prev_style = '\x00';
            for (grid_t x = screen_left; x != screen_right; ++x) {
                for (uint8_t tile_x = 0; tile_x < TILE_WIDTH; ++tile_x)
                    prev_style = apply_pixel(game, x, y, tile_x, tile_y,
                                             attackable, buildable, prev_style);
            }
            reset_style();
            wprintf(L"\n");
        }
    }

    print_text(game, attackable, buildable);
}
