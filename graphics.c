#include "graphics.h"
#include "constants.h"
#include "console.h"
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

#define tile_width 8
#define tile_height 4
#define unit_left 1
#define unit_top 1
#define unit_width 5
#define unit_height 2

#define unit_right (unit_left + unit_width - 1)
#define unit_bottom (unit_top + unit_height - 1)

#define accessible_style '\xe0'
#define attackable_style '\x90'
#define accessible_attackable_style '\xd0'
#define buildable_style '\xf0'

const static uint8_t unit_symbols[14] = {' ', '_',  'o', 'x', '<', '>', 'v',
                                         '^', '\\', '/', '[', ']', '-', '='};

const static uint8_t player_styles[players_capacity + 1] = {
    '\xf4', '\xf1', '\xf3', '\xf8', '\xf8', '\xf8'};

const static uint8_t tile_styles[terrian_capacity] = {
    '\x80', '\xa2', '\x32', '\x13', '\x3b',
    '\xc4', '\xd4', '\x4c', '\x78', '\x78'};
const static uint8_t
    unit_textures[model_capacity][unit_height][(unit_width + 1) / 2] = {
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

const static uint8_t
    capturable_textures[capturable_capacity][tile_height]
                       [(tile_width + 1) / 2] = {
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

void graphics_init() { setlocale(LC_CTYPE, ""); }

void render_block(uint32_t progress, uint32_t completion, const grid_t tile_x,
                  wchar_t* const symbol, uint8_t* const style) {
    assert(progress < completion);
    assert(completion > 0);
    const uint8_t styles[] = {'\x90', '\xb0', '\xa0'};
    const uint8_t inverted_styles[] = {'\x09', '\x0B', '\x0A'};
    const uint8_t style_index = (3 * progress) / completion;

    const wchar_t block_symbols[] = {L'▏', L'▎', L'▍', L'▌', L'▋', L'▊', L'▉'};
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

void render_percentage(uint32_t progress, uint32_t completion,
                       const grid_t tile_x, wchar_t* const symbol) {
    assert(progress < completion);
    assert(completion > 0);
    const uint8_t percent = (100 * progress) / completion;
    const wchar_t left_digit = '0' + percent / 10;
    const wchar_t right_digit = '0' + percent % 10;
    if (percent >= 50) {
        if (tile_x == unit_left) {
            assert(*symbol == ' ');
            *symbol = left_digit;
        } else if (tile_x == unit_left + 1) {
            assert(*symbol == ' ');
            *symbol = right_digit;
        }
    } else {
        if (tile_x == unit_right - 1) {
            assert(*symbol == ' ');
            *symbol = left_digit;
        } else if (tile_x == unit_right) {
            assert(*symbol == ' ');
            *symbol = right_digit;
        }
    }
}

void render_bar(uint32_t progress, uint32_t completion, const grid_t tile_x,
                wchar_t* const symbol, uint8_t* const style) {
    render_block(progress, completion, tile_x, symbol, style);
    render_percentage(progress, completion, tile_x, symbol);
}

bool render_unit_health_bar(const struct units* const units, const grid_t x,
                            const grid_t y, const grid_t tile_x,
                            const grid_t tile_y, wchar_t* const symbol,
                            uint8_t* const style) {

    // Display health bar on the bottom of unit
    if (tile_y != tile_height - 1)
        return false;

    if (tile_x < unit_left || tile_x > unit_right)
        return false;

    const struct unit* const unit = units_const_get_at(units, x, y);
    if (!unit)
        return false;

    const health_t health = unit->health;

    // Hide health bar on full-health units
    if (health == health_max)
        return false;

    render_bar(health, health_max, tile_x, symbol, style);

    return true;
}

bool render_capture_progress_bar(const struct units* const units,
                                 const grid_t x, const grid_t y,
                                 const grid_t tile_x, const grid_t tile_y,
                                 wchar_t* const symbol, uint8_t* const style) {

    // Display health bar on the bottom of capturable
    if (tile_y != 0)
        return false;

    if (tile_x < unit_left || tile_x >= unit_left + unit_width)
        return false;

    const struct unit* const unit = units_const_get_at(units, x, y - (grid_t)1);
    if (!unit)
        return false;

    const health_wide_t capture_progress = unit->capture_progress;

    // Hide health bar on full-health units
    if (capture_progress == 0)
        return false;

    render_bar(capture_progress, capture_completion, tile_x, symbol, style);

    return true;
}

uint8_t calc_tile_style(const struct game* const game, const grid_t x,
                        const grid_t y) {
    const tile_t tile = game->map[y][x];
    return tile < terrian_capacity ? tile_styles[tile]
                                   : player_styles[game->territory[y][x]];
}

uint8_t calc_action_style(const bool attack_enabled, const bool build_enabled) {
    assert(!(attack_enabled && build_enabled));
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
        if (player == null_player)
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
    if (unit_left > tile_x || tile_x > unit_right || unit_top > tile_y ||
        tile_y > unit_bottom)
        return false;

    const struct unit* const unit = units_const_get_at(units, x, y);
    if (!unit)
        return false;

    const bool transparent = decode_texture(
        unit_textures[unit->model][tile_y - unit_top][(tile_x - unit_left) / 2],
        (tile_x - unit_left) % 2 != 0, unit->player, symbol, style);

    if (transparent)
        return false;

    // Dim forecolours if disabled
    if (!unit->enabled)
        *style &= '\x0F';

    return true;
}

void render_highlight(const uint8_t label, wchar_t* const symbol,
                      uint8_t* const style) {

    assert(!(label & ~(accessible_bit | attackable_bit)));

    // Apply label hightlighting
    if (!label)
        return;

    // Clear foreground style
    *style &= '\x0f';
    *symbol = L'░';

    // Set foreground style
    switch (label) {
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

void render_attack_arrows(const struct game* const game, const grid_t tile_x,
                          wchar_t* const symbol, uint8_t* const style) {

    if (tile_x % 2 != 0) {
        *symbol = ' ';
    } else {
        if ((grid_t)(game->prev_x + 1) == game->x) {
            assert(game->prev_y == game->y);
            *symbol = L'▶';
        } else if ((grid_t)(game->prev_x - 1) == game->x) {
            assert(game->prev_y == game->y);
            *symbol = L'◀';
        } else if ((grid_t)(game->prev_y + 1) == game->y) {
            assert(game->prev_x == game->x);
            *symbol = L'▼';
        } else if ((grid_t)(game->prev_y - 1) == game->y) {
            assert(game->prev_x == game->x);
            *symbol = L'▲';
        } else
            // Previous position incorrectly set
            assert(false);

        // Set foreground colour to attackable style
        *style = (*style & '\x0f') | attackable_style;
    }
}

bool render_tile(const struct game* const game, const grid_t x, const grid_t y,
                 const grid_t tile_x, const grid_t tile_y,
                 const bool attack_enabled, wchar_t* const symbol,
                 uint8_t* const style) {

    const tile_t tile = game->map[y][x];
    bool highlightable = true;

    if (tile < terrian_capacity) {
        *style = tile_styles[tile];
        *symbol = tile_symbols[tile];
    } else {
        // If texture is transparent, texture is highlightable
        highlightable = decode_texture(
            capturable_textures[tile - terrian_capacity][tile_y][tile_x / 2],
            tile_x % 2 != 0, game->territory[y][x], symbol, style);

        if (highlightable)
            *symbol = ' ';
    }

    // Show arrows highlighting position to attack unit
    if (attack_enabled && x == game->prev_x && y == game->prev_y)
        render_attack_arrows(game, tile_x, symbol, style);
    else if (highlightable)
        render_highlight(game->labels[y][x], symbol, style);

    return true;
}

bool render_pixel(const struct game* const game, const grid_t x, const grid_t y,
                  const grid_t tile_x, const grid_t tile_y,
                  const bool attack_enabled, const bool build_enabled,
                  wchar_t* const symbol, uint8_t* const style) {
    return render_unit_health_bar(&game->units, x, y, tile_x, tile_y, symbol,
                                  style) ||
           render_capture_progress_bar(&game->units, x, y, tile_x, tile_y,
                                       symbol, style) ||
           render_selection(game, x, y, tile_x, tile_y, attack_enabled,
                            build_enabled, symbol, style) ||
           render_unit(&game->units, x, y, tile_x, tile_y, symbol, style) ||
           render_tile(game, x, y, tile_x, tile_y, attack_enabled, symbol,
                       style);
}

static void apply_style(const uint8_t style, const uint8_t prev_style) {
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
static uint8_t apply_pixel(const struct game* const game, const grid_t x,
                           const grid_t y, const grid_t tile_x,
                           const grid_t tile_y, const bool attack_enabled,
                           const bool build_enabled, const uint8_t prev_style) {
    wchar_t symbol;
    uint8_t style;
    const bool rendered =
        render_pixel(game, x, y, tile_x, tile_y, attack_enabled, build_enabled,
                     &symbol, &style);
    assert(rendered);
    apply_style(style, prev_style);
    wprintf(L"%lc", symbol);
    return style;
}

static void reset_cursor() {
    wprintf(L"\033[0;0H");
    wprintf(L"\033[2J\033[1;1H");
}

static void reset_black() { wprintf(L"%c[30;40m", '\x1b'); }

static void reset_style() { wprintf(L"%c[0m", '\x1b'); }

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
    // TODO: fix attack damage not matching resultant printed health
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

void graphics_render(const struct game* const game, const bool attack_enabled,
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
                for (uint8_t tile_x = 0; tile_x < tile_width; ++tile_x)
                    prev_style =
                        apply_pixel(game, x, y, tile_x, tile_y, attack_enabled,
                                    build_enabled, prev_style);
            }
            reset_style();
            wprintf(L"\n");
        }
    }

    print_text(game, attack_enabled, build_enabled);
}