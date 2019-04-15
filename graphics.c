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
					tile_index tile = game->map[game->y + grid_y][game->x + grid_x];
					uint8_t symbol = tile_symbols[tile];
					uint8_t style = tile_styles[tile];
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
