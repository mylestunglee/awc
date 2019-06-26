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
	const grid_index grid_x,
	const grid_index grid_y,
	uint8_t* const symbol,
	uint8_t* const style) {

	// Out of bounds
	if (unit_left > grid_x || grid_x >= unit_left + unit_width ||
		unit_top > grid_y || grid_y >= unit_top + unit_height)
		return false;

	const unit_index unit = game->units.grid[y][x];

	if (unit == null_unit)
		return false;

	uint8_t texture = unit_textures[unit]
		[grid_y - unit_top][(grid_x - unit_left) / 2];

	// Extract 4-bits corresponding to texture coordinate
	if ((grid_x - unit_left) % 2 == 0)
		texture = texture >> 4;
	else
		texture = texture & 15;

	if (texture == 0)
		return false;

	unit_type player = unit_get_player(&game->units.data[unit]);
	*style = player_style[player];

	// Highlight selected unit
	if (game->selected == unit) {
		*style ^= 0x88;
	}

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
	const grid_index grid_x,
	const grid_index grid_y,
	uint8_t* const symbol,
	uint8_t* const style) {

	if (game->x != x || game->y != y)
		return false;

	// Construct ASCII box art in selected tile
	uint8_t edges = 0;

	if (grid_x == 0) ++edges;
	if (grid_x == grid_width - 1) ++edges;
	if (grid_y == 0) edges += 2;
	if (grid_y == grid_height - 1) edges += 2;

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
			//*symbol = '+';
			*symbol = game->workspace[y][x] + '0';
			break;
		}
	}

	const grid_index grid = game->map[y][x];
	*style = (grid_styles[grid] & '\x0f') | selection_style;

 	return true;
}

static bool render_grid(
	const struct game* const game,
	const grid_index x,
	const grid_index y,
	uint8_t* const symbol,
	uint8_t* const style) {

	const grid_index grid = game->map[y][x];
	*symbol = grid_symbols[grid];
	*style = grid_styles[grid];

	// Apply label hightlighting
	if (game->labels[y][x] != 0) {
		// Clear foreground style
		*style &= '\x0f';
		*symbol = '#';

		// Set foreground style
		switch (game->labels[y][x]) {
			case accessible_bit: {
				*style |= accessible_style;
				break;
			}
			case attackable_bit: {
				*style |= attackable_style;
				break;
			}
			case accessible_bit | attackable_bit: {
				*style |= both_style;
				break;
			}
		}
	}

	return true;
}

static void reset_cursor() {
	printf("\033[0;0H");
	printf("\033[2J\033[1;1H");
}

void render(const struct game* const game) {
	reset_cursor();

	const grid_index screen_left = game->x - screen_width / 2 + 1;
	const grid_index screen_right = game->x + screen_width / 2 + 1;
	const grid_index screen_top = game->y - screen_height / 2 + 1;
	const grid_index screen_bottom = game->y + screen_height / 2 + 1;

	for (grid_index y = screen_top; y != screen_bottom; ++y) {
		for (grid_index grid_y = 0; grid_y < grid_height; ++grid_y) {
			uint8_t prev_style = '\0';
			for (grid_index x = screen_left; x != screen_right; ++x) {
				for (grid_index grid_x = 0; grid_x < grid_width; ++grid_x) {
					uint8_t symbol;
					uint8_t style;

					if (render_selection(game, x, y, grid_x, grid_y, &symbol, &style) ||
						render_unit(game, x, y, grid_x, grid_y, &symbol, &style) ||
						render_grid(game, x, y, &symbol, &style)) {

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
