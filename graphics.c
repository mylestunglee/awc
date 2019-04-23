#include <stdio.h>
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

// Attempt to find symbol style pair from rendering coordinates
static bool render_unit(
	const struct game* const game,
	const grid_index x,
	const grid_index y,
	const grid_index tile_x,
	const grid_index tile_y,
	uint8_t* const symbol,
	uint8_t* const style) {

	if (unit_left > tile_x || tile_x >= unit_right ||
		unit_top > tile_y || tile_y >= unit_bottom)
		return false;

	const unit_index unit = game->units.grid[y][x];

	if (unit == null_unit)
		return false;

	uint8_t texture = unit_textures[unit]
		[tile_y - unit_top][(tile_x - unit_left) / 2];

	if ((tile_x - unit_left) % 2 == 0)
		texture = texture >> 4;
	else
		texture = texture & 15;

	if (texture == 0)
		return false;

	unit_type player = unit_get_player(&game->units.units[unit]);
	*style = player_style[player];

	if (texture == 15)
		*symbol = player_symbol[player];
	else
		*symbol = unit_symbols[texture - 1];

	return true;
}

static bool render_selection(
	const struct game* const game,
	const grid_index x,
	const grid_index y,
	const grid_index tile_x,
	const grid_index tile_y,
	uint8_t* const symbol,
	uint8_t* const style) {

	if (game->x != x || game->y != y)
		return false;

	if (0 < tile_x && tile_x < tile_width - 1 &&
		0 < tile_y && tile_y < tile_height - 1)
		return false;

	*symbol = selection_symbol;
	*style = selection_style;

	return true;
}

static bool render_tile(
	const struct game* const game,
	const grid_index x,
	const grid_index y,
	uint8_t* const symbol,
	uint8_t* const style) {

	tile_index tile = game->map[y][x];
	*symbol = tile_symbols[tile];
	*style = tile_styles[tile];

	return true;
}

static void reset_cursor() {
	printf("\e[0;0H");
}

void render(const struct game* const game) {
	reset_cursor();
	grid_index screen_left;
	grid_index screen_right;
	grid_index screen_top;
	grid_index screen_bottom;

	if (game->x < screen_half_width) {
		screen_left = 0;
		screen_right = screen_width;
	} else if (game->x >= grid_size - screen_half_width) {
		screen_left = grid_size - screen_width;
		screen_right = 255;
	} else {
		screen_left = game->x - screen_half_width;
		screen_right = game->x + screen_half_width;
	}

	screen_top = 0;
	screen_bottom = 4;

	for (grid_index y = screen_top; y < screen_bottom; ++y) {
		for (grid_index tile_y = 0; tile_y < tile_height; ++tile_y) {
			uint8_t prev_style = '\0';
			for (grid_index x = screen_left; x < screen_right; ++x) {
				for (grid_index tile_x = 0; tile_x < tile_width; ++tile_x) {
					uint8_t symbol;
					uint8_t style;

					if (render_selection(game, x, y, tile_x, tile_y, &symbol, &style) ||
						render_unit(game, x, y, tile_x, tile_y, &symbol, &style) ||
						render_tile(game, x, y, &symbol, &style)) {

						render_pixel(symbol, style, prev_style);
						prev_style = style;
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
