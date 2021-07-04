#include "graphics.h"
#include "console.h"
#include <locale.h>
#include <stdio.h>
#include <wchar.h>

#define tile_width 8
#define tile_height 4
#define unit_left 1
#define unit_top 1
#define unit_width 5
#define unit_height 2

#define selection_symbol '+'
#define accessible_style '\xe0'
#define attackable_style '\x90'
#define accessible_attackable_style '\xd0'
#define buildable_style '\xf0'

#define vertical_bit 1
#define horizontal_bit 2

const static uint8_t unit_symbols[14] = {' ', '_',  'o', 'x', '<', '>', 'v',
                                         '^', '\\', '/', '[', ']', '-', '='};
const static uint8_t player_styles[players_capacity + 1] = {
    '\xF4', '\xF1', '\xF3', '\xF8', '\xF8', '\xF8'};

const static uint8_t tile_styles[terrian_capacity] = {
    '\x80', '\xA2', '\x32', '\x13', '\x3B',
    '\xC4', '\xD4', '\x4C', '\x78', '\x78'};
const static uint8_t
    unit_textures[model_capacity][unit_height][(unit_width + 1) / 2] = {
        {{'\x03', '\xF3', '\x00'}, {'\x08', '\x88', '\x00'}},
        {{'\x0E', '\xFE', '\x00'}, {'\x08', '\x88', '\x00'}},
        {{'\x0A', '\xFD', '\x00'}, {'\x03', '\x33', '\x00'}},
        {{'\xAB', '\xFC', '\xD0'}, {'\x3E', '\xEE', '\x30'}},
        {{'\xB1', '\xFC', '\xE0'}, {'\x3E', '\xEE', '\x30'}},
        {{'\x08', '\xF8', '\x00'}, {'\x03', '\xE3', '\x00'}},
        {{'\x88', '\xF8', '\x80'}, {'\x33', '\x33', '\x30'}},
        {{'\x0A', '\xFA', '\x00'}, {'\x03', '\xE3', '\x00'}},
        {{'\xAA', '\xFA', '\xA0'}, {'\x33', '\x33', '\x30'}},
        {{'\x0E', '\x4E', '\x10'}, {'\x0B', '\xFC', '\xE0'}},
        {{'\x91', '\x11', '\x10'}, {'\x59', '\xFC', '\xE0'}},
        {{'\x91', '\x11', '\x10'}, {'\x57', '\xF7', '\x60'}},
        {{'\x22', '\x22', '\x20'}, {'\x92', '\xFC', '\xE0'}},
        {{'\x2A', '\xFA', '\x20'}, {'\x92', '\x2C', '\xE0'}},
        {{'\x88', '\xF8', '\x80'}, {'\x92', '\x22', '\xA0'}}};

const static uint8_t
    capturable_textures[capturable_capacity][tile_height]
                       [(tile_width + 1) / 2] = {
                           {
                               {'\x00', '\xB1', '\xFC', '\x00'},
                               {'\x00', '\xB1', '\xB1', '\x1C'},
                               {'\xB1', '\x1C', '\xB1', '\x1C'},
                               {'\xB1', '\x1C', '\x00', '\x00'},
                           },
                           {
                               {'\x00', '\x00', '\x00', '\x00'},
                               {'\xB9', '\x2B', '\xF2', '\xAC'},
                               {'\xB1', '\x11', '\x11', '\x1C'},
                               {'\xB1', '\x11', '\x11', '\x1C'},
                           },
                           {
                               {'\x00', '\x00', '\x9F', '\xA0'},
                               {'\x00', '\x00', '\xC1', '\xB0'},
                               {'\x22', '\x22', '\xC1', '\xB2'},
                               {'\xB1', '\x11', '\x11', '\x1C'},
                           },
                           {
                               {'\x00', '\x00', '\x00', '\x00'},
                               {'\x00', '\x00', '\xBC', '\xFC'},
                               {'\x22', '\x22', '\xBC', '\x1C'},
                               {'\xB1', '\x11', '\x11', '\x1C'},
                           },
                           {{'\x00', '\xBE', '\xFC', '\x00'},
                            {'\x00', '\xB1', '\x1C', '\x00'},
                            {'\xB1', '\x11', '\x11', '\x1C'},
                            {'\xB1', '\x11', '\x11', '\x1C'}}};

static void render_pixel(wchar_t symbol, const uint8_t style,
                         const uint8_t prev_style) {
    if (style != prev_style) {
        const uint8_t forecolour = style >> 4;
        const uint8_t backcolour = style & 15;
        const uint8_t prev_forecolour = prev_style >> 4;
        const uint8_t prev_backcolour = prev_style & 15;
        bool carry = false;

        wprintf(L"%c[", '\x1B');
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
    wprintf(L"%lc", symbol);
}

// Returns true iff texture is transparent
bool decode_texture(const uint8_t textures, const bool polarity,
                    const player_t player, wchar_t* const symbol,
                    uint8_t* const style) {

    *style = player_styles[player];
    // Extract 4-bits corresponding to texture coordinate
    const uint8_t texture = polarity ? (textures >> 4) : (textures & '\x0f');

    // Handle transparent pixel
    if (texture == '\x00')
        return true;
    else if (texture == '\x0f') {
        if (player == null_player)
            *symbol = ' ';
        else
            *symbol = '1' + player;
    } else
        *symbol = unit_symbols[texture - 1];

    return false;
}

// Attempt to find symbol style pair from rendering coordinates
bool render_unit(const struct game* const game, const grid_t x, const grid_t y,
                 const grid_t tile_x, const grid_t tile_y,
                 wchar_t* const symbol, uint8_t* const style) {

    // Out of bounds
    if (unit_left > tile_x || tile_x >= unit_left + unit_width ||
        unit_top > tile_y || tile_y >= unit_top + unit_height)
        return false;

    const struct unit* const unit = units_const_get_at(&game->units, x, y);
    if (!unit)
        return false;

    const uint8_t textures =
        unit_textures[unit->model][tile_y - unit_top][(tile_x - unit_left) / 2];

    const bool transparent = decode_texture(
        textures, (tile_x - unit_left) % 2 == 0, unit->player, symbol, style);

    if (transparent)
        return false;

    // Dim forecolours if disabled
    if (!unit->enabled)
        *style &= '\x0F';

    return true;
}

uint8_t calc_tile_style(const struct game* const game, const grid_t x,
                        const grid_t y) {
    const tile_t tile = game->map[y][x];

    if (tile < terrian_capacity)
        return tile_styles[tile];
    else
        return player_styles[game->territory[y][x]];
}

uint8_t calc_action_style(const bool attack_enabled, const bool build_enabled) {
    if (attack_enabled)
        return attackable_style;
    else if (build_enabled)
        return buildable_style;
    else
        return accessible_style;
}

uint8_t calc_selection_style(const struct game* const game, const grid_t x,
                             const grid_t y, const bool attack_enabled,
                             const bool build_enabled) {
    return (calc_tile_style(game, x, y) & '\x0f') |
           calc_action_style(attack_enabled, build_enabled);
}

bool calc_selection_symbol(const grid_t tile_x, const grid_t tile_y,
                           wchar_t* const symbol) {
    const bool top = tile_y == 0;
    const bool bottom = tile_y == tile_height - 1;
    const bool left = tile_x == 0;
    const bool right = tile_x == tile_width - 1;

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
                      const bool attack_enabled, const bool build_enabled,
                      wchar_t* const symbol, uint8_t* const style) {
    if (game->x != x || game->y != y)
        return false;

    const bool selected = calc_selection_symbol(tile_x, tile_y, symbol);

    if (selected)
        *style =
            calc_selection_style(game, x, y, attack_enabled, build_enabled);

    return selected;
}

static void render_block(uint32_t progress, uint32_t completion,
                         const grid_t tile_x, wchar_t* const symbol,
                         uint8_t* const style) {
    const uint8_t styles[3] = {'\x90', '\xB0', '\xA0'};
    const uint8_t inverted_styles[3] = {'\x09', '\x0B', '\x0A'};
    const uint8_t style_index = (3 * progress) / completion;

    const wchar_t block_symbols[8] = {L'▏', L'▎', L'▍', L'▌', L'▋', L'▊', L'▉'};
    const int8_t steps = ((8 * unit_width + 1) * progress) / completion -
                         8 * (tile_x - unit_left) - 1;
    if (steps < 0) {
        *style = styles[style_index];
        *symbol = ' ';
    } else if (steps < 7) {
        *style = styles[style_index];
        *symbol = block_symbols[steps];
    } else {
        *style = inverted_styles[style_index];
        *symbol = ' ';
    }
}

static void render_percent(uint32_t progress, uint32_t completion,
                           const grid_t tile_x, wchar_t* const symbol) {
    const uint8_t percent = (100 * progress) / completion;
    assert(percent > 0);
    assert(percent < 100);
    const wchar_t left_digit = '0' + percent / 10;
    const wchar_t right_digit = '0' + percent % 10;
    if (percent >= 50) {
        if (tile_x == unit_left)
            *symbol = left_digit;
        else if (tile_x == unit_left + 1)
            *symbol = right_digit;
    } else {
        if (tile_x == unit_left + unit_width - 2)
            *symbol = left_digit;
        else if (tile_x == unit_left + unit_width - 1)
            *symbol = right_digit;
    }
}

// Set health bar colour
static void render_bar(uint32_t progress, uint32_t completion,
                       const grid_t tile_x, wchar_t* const symbol,
                       uint8_t* const style) {
    render_block(progress, completion, tile_x, symbol, style);
    render_percent(progress, completion, tile_x, symbol);
}

static bool render_health_bar(const struct game* const game, const grid_t x,
                              const grid_t y, const grid_t tile_x,
                              const grid_t tile_y, wchar_t* const symbol,
                              uint8_t* const style) {

    // Display health bar on the bottom of unit
    if (tile_y != tile_height - 1)
        return false;

    if (tile_x < unit_left || tile_x >= unit_left + unit_width)
        return false;

    const struct unit* const unit = units_const_get_at(&game->units, x, y);
    if (!unit)
        return false;

    const health_t health = unit->health;

    // Hide health bar on full-health units
    if (health == health_max)
        return false;

    render_bar(health, health_max, tile_x, symbol, style);

    return true;
}

static bool render_capture_progress_bar(const struct game* const game,
                                        const grid_t x, const grid_t y,
                                        const grid_t tile_x,
                                        const grid_t tile_y,
                                        wchar_t* const symbol,
                                        uint8_t* const style) {

    // Display health bar on the bottom of unit
    if (tile_y != 0)
        return false;

    if (tile_x < unit_left || tile_x >= unit_left + unit_width)
        return false;

    const struct unit* const unit =
        units_const_get_at(&game->units, x, y - (grid_t)1);
    if (!unit)
        return false;

    const health_wide_t capture_progress = unit->capture_progress;

    // Hide health bar on full-health units
    if (capture_progress == 0)
        return false;

    render_bar(capture_progress, capture_completion, tile_x, symbol, style);

    return true;
}

static void render_highlight(const struct game* const game, const grid_t x,
                             const grid_t y, wchar_t* const symbol,
                             uint8_t* const style) {

    const uint8_t highlight =
        game->labels[y][x] & (accessible_bit | attackable_bit);

    // Apply label hightlighting
    if (!highlight)
        return;

    // Clear foreground style
    *style &= '\x0f';
    *symbol = L'░'; // light shade

    // Set foreground style
    switch (highlight) {
    case accessible_bit:
        *style |= accessible_style;
        break;
    case attackable_bit:
        *style |= attackable_style;
        break;
    case accessible_bit | attackable_bit:
        *style |= accessible_attackable_style;
        break;
    }
}

static bool render_tile(const struct game* const game, const grid_t x,
                        const grid_t y, const grid_t tile_x,
                        const grid_t tile_y, const bool attack_enabled,
                        wchar_t* const symbol, uint8_t* const style) {

    const tile_t tile = game->map[y][x];
    bool highlightable = true;

    if (tile < terrian_capacity) {
        *style = tile_styles[tile];
        *symbol = tile_symbols[tile];
    } else {
        highlightable = decode_texture(
            capturable_textures[tile - terrian_capacity][tile_y][tile_x / 2],
            tile_x % 2 == 0, game->territory[y][x], symbol, style);

        if (highlightable)
            *symbol = ' ';
    }

    // Show arrows highlighting position to attack unit
    if (attack_enabled && x == game->prev_x && y == game->prev_y) {
        if (tile_x % 2 != 0) {
            *symbol = ' ';
        } else {
            if ((grid_t)(game->prev_x + 1) == game->x)
                *symbol = L'▶';
            else if ((grid_t)(game->prev_x - 1) == game->x)
                *symbol = L'◀';
            else if ((grid_t)(game->prev_y + 1) == game->y)
                *symbol = L'▼';
            else if ((grid_t)(game->prev_y - 1) == game->y)
                *symbol = L'▲';
            else
                // Previous position incorrectly set
                assert(false);

            // Set foreground colour to attackable style
            *style = (*style & '\x0f') | attackable_style;
        }
    } else if (highlightable)
        render_highlight(game, x, y, symbol, style);

    return true;
}

static void reset_cursor() {
    wprintf(L"\033[0;0H");
    wprintf(L"\033[2J\033[1;1H");
}

static void reset_black() { wprintf(L"%c[30;40m", '\x1B'); }

static void print_normal_text(const struct game* const game) {
    wprintf(
        L"turn=%hhu x=%hhu y=%hhu tile=%s territory=%hhu label=%u gold=%u\n",
        game->turn, game->x, game->y, tile_names[game->map[game->y][game->x]],
        game->territory[game->y][game->x], game->labels[game->y][game->x],
        game->golds[game->turn]);

    const struct unit* unit =
        units_const_get_at(&game->units, game->x, game->y);
    if (unit)
        wprintf(L"unit health=" health_format " model=%s capture_progress=%u",
                unit->health, model_names[unit->model], unit->capture_progress);
}

static void print_attack_text(const struct game* const game) {
    health_t damage, counter_damage;
    game_simulate_attack(game, &damage, &counter_damage);
    const health_wide_t percent = 100;
    wprintf(L"Damage: %u%% Counter-damage: %u%%\n",
            (damage * percent) / health_max,
            (counter_damage * percent) / health_max);
}

static void print_build_text(const struct game* const game) {
    const tile_t tile = game->map[game->y][game->x];
    assert(tile >= terrian_capacity);
    const tile_t capturable = tile - terrian_capacity;

    wprintf(L"in build mode:");
    for (model_t model = buildable_models[capturable];
         model < buildable_models[capturable + 1]; ++model) {
        wprintf(L"(" model_format ") %s ", model + 1, model_names[model]);
    }
    wprintf(L"\n");
}

static void print_text(const struct game* const game, const bool attack_enabled,
                       const bool build_enabled) {
    if (attack_enabled)
        print_attack_text(game);
    else if (build_enabled)
        print_build_text(game);
    else
        print_normal_text(game);
}

void render(const struct game* const game, const bool attack_enabled,
            const bool build_enabled) {
    reset_cursor();

    int console_width, console_height;
    get_console_size(&console_width, &console_height);
    const grid_t screen_width = console_width / tile_width;
    const grid_t screen_height = console_height / tile_height;
    const grid_t screen_left = game->x - screen_width / 2 + 1;
    const grid_t screen_right = game->x + screen_width / 2 + 1;
    const grid_t screen_top = game->y - screen_height / 2;
    const grid_t screen_bottom = game->y + screen_height / 2;

    for (grid_t y = screen_top; y != screen_bottom; ++y) {
        for (uint8_t tile_y = 0; tile_y < tile_height; ++tile_y) {
            reset_black();
            uint8_t prev_style = '\x00';
            for (grid_t x = screen_left; x != screen_right; ++x) {
                for (uint8_t tile_x = 0; tile_x < tile_width; ++tile_x) {
                    wchar_t symbol;
                    uint8_t style;

                    if (render_health_bar(game, x, y, tile_x, tile_y, &symbol,
                                          &style) ||
                        render_capture_progress_bar(game, x, y, tile_x, tile_y,
                                                    &symbol, &style) ||
                        render_selection(game, x, y, tile_x, tile_y,
                                         attack_enabled, build_enabled, &symbol,
                                         &style) ||
                        render_unit(game, x, y, tile_x, tile_y, &symbol,
                                    &style) ||
                        render_tile(game, x, y, tile_x, tile_y, attack_enabled,
                                    &symbol, &style)) {
                        render_pixel(symbol, style, prev_style);
                        prev_style = style;
                    } else {
                        assert(false);
                    }
                }
            }
            reset_style();
            wprintf(L"\n");
        }
    }

    print_text(game, attack_enabled, build_enabled);
}

#include <stdlib.h>

void reset_style() { wprintf(L"%c[0m", '\x1B'); }

void graphics_init() { setlocale(LC_CTYPE, ""); }