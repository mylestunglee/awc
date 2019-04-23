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

void render(const struct game* const game) {
	for (grid_index grid_y = 0; grid_y < screen_height; ++grid_y) {
		for (grid_index tile_y = 0; tile_y < tile_height; ++tile_y) {
			uint8_t prev_style = '\0';
			for (grid_index grid_x = 0; grid_x < screen_height; ++grid_x) {
				for (grid_index tile_x = 0; tile_x < tile_width; ++tile_x) {

grid_index x = game->x + grid_x;
grid_index y = game->y + grid_y;

uint8_t symbol;
uint8_t style;
bool found = false;

if (unit_left <= tile_x && tile_x < unit_right &&
	unit_top <= tile_y && tile_y < unit_bottom) {
	unit_index unit = game->units.grid[y][x];
	if (unit != null_unit) {
		uint8_t texture = unit_textures[unit]
			[tile_y - unit_top][(tile_x - unit_left) / 2];

		if ((tile_x - unit_left) % 2 == 0) {
			texture = texture >> 4;
		} else {
			texture = texture & 15;
		}

		if (texture != 0 && texture != 15) {
			symbol = unit_symbols[texture - 1];
			style = player_style[unit_get_player(&game->units.units[unit])];
			found = true;
		}

		if (texture == 15) {
			symbol = player_symbol[unit_get_player(&game->units.units[unit])];
			style = player_style[unit_get_player(&game->units.units[unit])];
			found = true;
		}
	}
}


tile_index tile = game->map[game->y + grid_y][game->x + grid_x];
if (!found) {
	symbol = tile_symbols[tile];
	style = tile_styles[tile];
}

render_pixel(symbol, style, prev_style);
prev_style = style;

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
