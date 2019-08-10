#include <stdio.h>
#include "bitarray.h"
#include "graphics.h"

static void render_pixel(uint8_t symbol, const uint8_t style, const uint8_t prev_style) {
	if (style != prev_style) {
		const uint8_t forecolour = style >> 4;
		const uint8_t backcolour = style & 15;
		const uint8_t prev_forecolour = prev_style >> 4;
		const uint8_t prev_backcolour = prev_style & 15;
		bool carry = false;

		printf("%c[", '\x1B');
		if (forecolour != prev_forecolour) {
			carry = true;
			if (forecolour < 8)
				printf("%u", forecolour + 30);
			else
				printf("%u", forecolour + 82);
		}
		if (backcolour != prev_backcolour) {
			if (carry)
				printf(";");
			if (backcolour < 8)
				printf("%u", backcolour + 40);
			else
				printf("%u", backcolour + 92);
		}
		printf("m");
	}
	printf("%c", symbol);
}

static bool hidden_tile(const struct game* const game, const grid_t x, const grid_t y) {
	return game->fog && !(game->labels[y][x] & reveal_bit);
}

// Attempt to find symbol style pair from rendering coordinates
static bool render_unit(
	const struct game* const game,
	const grid_t x,
	const grid_t y,
	const grid_t tile_x,
	const grid_t tile_y,
	uint8_t* const symbol,
	uint8_t* const style) {

	// Hide in fog
	if (hidden_tile(game, x, y))
		return false;

	// Out of bounds
	if (unit_left > tile_x || tile_x >= unit_left + unit_width ||
		unit_top > tile_y || tile_y >= unit_top + unit_height)
		return false;

	const unit_t unit_index = game->units.grid[y][x];

	if (unit_index == null_unit)
		return false;

	const struct unit* const unit = &game->units.data[unit_index];

	const model_t model = unit->model;
	uint8_t texture = unit_textures[model]
		[tile_y - unit_top][(tile_x - unit_left) / 2];

	// Extract 4-bits corresponding to texture coordinate
	if ((tile_x - unit_left) % 2 == 0)
		texture >>= 4;
	else
		texture &= '\x0F';

	// Transparent pixel
	if (texture == 0)
		return false;

	if (texture == '\x0F')
		*symbol = player_symbols[unit->player];
	else
		*symbol = unit_symbols[texture - 1];

	*style = player_styles[unit->player];

	// Dim forecolours if disable
	if (!unit->enabled)
		*style &= '\x0F';

	return true;
}

static bool render_selection(
	const struct game* const game,
	const grid_t x,
	const grid_t y,
	const grid_t tile_x,
	const grid_t tile_y,
	const bool attack_enabled,
	const bool build_enabled,
	uint8_t* const symbol,
	uint8_t* const style) {

	if (game->x != x || game->y != y)
		return false;

	// Construct ASCII box art in selected tile
	uint8_t edges = 0;

	if (tile_x == 0) ++edges;
	if (tile_x == tile_width - 1) ++edges;
	if (tile_y == 0) edges += 2;
	if (tile_y == tile_height - 1) edges += 2;

	switch (edges) {
		case 0: {
			return false;
		}
		case 1: {
			*symbol = '|';
			break;
		}
		case 2: {
			*symbol = '-';
			break;
		}
		case 3: {
			*symbol = '+';
			break;
		}
	}

	// Darken background in fog
	if (hidden_tile(game, x, y))
		*style = tile_styles[0];
	else {
		const tile_t tile = game->map[y][x];

		// Handle terrian and capturable tiles
		if (tile < terrian_capacity)
			*style = tile_styles[tile];
		else
			*style = player_styles[game->territory[y][x]];
	}

	// Highlight box pre-attack
	*style &= '\x0f';

	if (attack_enabled)
		*style |= attackable_style;
	else if (build_enabled)
		*style |= buildable_style;
	else
		*style |= accessible_style;

 	return true;
}

static bool render_health_bar(
	const struct game* const game,
	const grid_t x,
	const grid_t y,
	const grid_t tile_x,
	const grid_t tile_y,
	uint8_t* const symbol,
	uint8_t* const style) {

	// Hide in fog
	if (hidden_tile(game, x, y))
		return false;

	// Display health bar on the bottom of unit
	if (tile_y != tile_height - 1)
		return false;

	if (tile_x < unit_left || tile_x >= unit_left + unit_width)
		return false;

	const unit_t unit_index = game->units.grid[y][x];

	if (unit_index == null_unit)
		return false;

	const struct unit* const unit = &game->units.data[unit_index];
	const health_t health = unit->health;

	// Hide health bar on full-health units
	if (health == health_max)
		return false;


	// Set health bar colour
	const uint8_t styles[4] = {'\x90', '\x30', '\xB0', '\xA0'};
	*style = styles[(health & '\xC0') >> 6];

	const uint8_t steps = (health_wide_t)health * (4 * unit_width - 1) / health_max;

	if (steps < 4 * (tile_x - unit_left))
		*symbol = ' ';
	else if (steps < 4 * (tile_x - unit_left) + 1)
		*symbol = '[';
	else if (steps < 4 * (tile_x - unit_left) + 2)
		*symbol = '|';
	else if (steps < 4 * (tile_x - unit_left) + 3)
		*symbol = ']';
	else
		*symbol = '=';

	return true;
}

static void render_highlight(
	const struct game* const game,
	const grid_t x,
	const grid_t y,
	const bool attack_enabled,
	uint8_t* const symbol,
	uint8_t* const style) {

	const uint8_t highlight = game->labels[y][x] & (accessible_bit | attackable_bit);

	// Apply label hightlighting
	if (!highlight)
		return;

	// Clear foreground style
	*style &= '\x0f';

	// Show arrows highlighting position to attack unit
	if (attack_enabled && x == game->prev_x && y == game->prev_y) {
		if ((grid_t)(game->prev_x + 1) == game->x)
			*symbol = '>';
		else if ((grid_t)(game->prev_x - 1) == game->x)
			*symbol = '<';
		else if ((grid_t)(game->prev_y + 1) == game->y)
			*symbol = 'v';
		else if ((grid_t)(game->prev_y - 1) == game->y)
			*symbol = '^';
		else
			// Previous position incorrectly set
			assert(false);
	} else
		*symbol = ':';

	// Set foreground style
	switch (highlight) {
		case accessible_bit:
			*style |= accessible_style;
			break;
		case attackable_bit:
			*style |= attackable_style;
			break;
	}
}

static bool render_terrian(
	const struct game* const game,
	const grid_t x,
	const grid_t y,
	const bool attack_enabled,
	uint8_t* const symbol,
	uint8_t* const style) {

	const tile_t tile = game->map[y][x];

	*symbol = tile_symbols[tile];

	if (hidden_tile(game, x, y))
		*style = tile_styles[0];
	else
		*style = tile_styles[tile];

	render_highlight(game, x, y, attack_enabled, symbol, style);

	return true;
}

static bool render_capturable(
	const struct game* const game,
	const grid_t x,
	const grid_t y,
	const grid_t tile_x,
	const grid_t tile_y,
	const bool attack_enabled,
	uint8_t* const symbol,
	uint8_t* const style) {

	const tile_t tile = game->map[y][x];
	uint8_t texture = capturable_textures[tile - terrian_capacity][tile_y][tile_x / 2];

	// Select left or right 4-bit texture
	if (tile_x % 2 == 0)
		texture >>= 4;
	else
		texture &= '\x0F';

	const player_t player = game->territory[y][x];

	if (hidden_tile(game, x, y))
		*style = tile_styles[0];
	else
		*style = player_styles[player];

	if (texture == 0) {
		*symbol = ' ';
		render_highlight(game, x, y, attack_enabled, symbol, style);
	} else if (texture == '\x0F') {
		// Show player symbol if tile is not hidden
		if (hidden_tile(game, x, y))
			*symbol = fog_symbol;
		else
			*symbol = player_symbols[player];
	} else
		*symbol = unit_symbols[texture - 1];

	return true;
}

static bool render_tile(
	const struct game* const game,
	const grid_t x,
	const grid_t y,
	const grid_t tile_x,
	const grid_t tile_y,
	const bool attack_enabled,
	uint8_t* const symbol,
	uint8_t* const style) {

	const tile_t tile = game->map[y][x];

	if (tile < terrian_capacity)
		return render_terrian(game, x, y, attack_enabled, symbol, style);
	else
		return render_capturable(game, x, y, tile_x, tile_y, attack_enabled, symbol, style);
}

static void reset_cursor() {
	printf("\033[0;0H");
	printf("\033[2J\033[1;1H");
}

static void reset_black() {
	printf("%c[30;40m", '\x1B');
}

void render(
	const struct game* const game,
	const bool attack_enabled,
	const bool build_enabled) {
	reset_cursor();

	const grid_t screen_left = game->x - screen_width / 2 + 1;
	const grid_t screen_right = game->x + screen_width / 2 + 1;
	const grid_t screen_top = game->y - screen_height / 2 + 1;
	const grid_t screen_bottom = game->y + screen_height / 2 + 1;

	for (grid_t y = screen_top; y != screen_bottom; ++y) {
		for (uint8_t tile_y = 0; tile_y < tile_height; ++tile_y) {
			reset_black();
			uint8_t prev_style = '\x00';
			for (grid_t x = screen_left; x != screen_right; ++x) {
				for (uint8_t tile_x = 0; tile_x < tile_width; ++tile_x) {
					uint8_t symbol;
					uint8_t style;

					if (render_health_bar(game, x, y, tile_x, tile_y, &symbol, &style) ||
						render_selection(game, x, y, tile_x, tile_y, attack_enabled, build_enabled, &symbol, &style) ||
						render_unit(game, x, y, tile_x, tile_y, &symbol, &style) ||
						render_tile(game, x, y, tile_x, tile_y, attack_enabled, &symbol, &style)) {

						render_pixel(symbol, style, prev_style);
						prev_style = style;
					} else {
						assert(false);
					}
				}
			}
			reset_style();
			printf("\n");
		}
	}
}

void reset_style() {
	printf("%c[0m", '\x1B');
}

