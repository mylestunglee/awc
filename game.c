#include <stdio.h>
#include "console.h"
#include "game.h"
#include "graphics.h"

static void game_map_initialise(tile_index map[grid_size][grid_size]) {
	grid_index y = 0;
	do {
		grid_index x = 0;
		do {
			map[y][x] = 0;
		} while (++x);
	} while (++y);
	map[0][1] = 1;
	map[1][2] = 2;
	map[0][16] = 1;
	map[0][196] = 1;
	map[0][255] = 2;
	map[2][255] = 2;
	map[3][255] = 2;
}

void game_initialise(struct game* game) {
	// TODO: fix order
	game->x = 0;
	game->y = 0;
	game_map_initialise(game->map);
	units_initialise(&game->units);
	game->selected = null_unit;
}

void game_loop(struct game* game) {
	render(game);

	char input = getch();

	while (input != 'q') {
		if (input == 'a') {
			--game->x;
		}
		if (input == 'd') {
			++game->x;
		}
		if (input == 'w') {
			--game->y;
		}
		if (input == 's') {
			++game->y;
		}
		if (input == ' ') {
			const unit_index unit = game->units.grid[game->y][game->x];
			if (game->selected == null_unit) {
				if (unit != null_unit) {
					game->selected = unit;
				}
			} else {
				// Cursor over selected unit
				if (game->selected == unit) {
					game->selected = null_unit;
				} else {
					units_move(&game->units, game->selected, game->x, game->y);
					game->selected = null_unit;
				}
			}
		}

		render(game);

		printf("%u %u", game->x, game->y);

		input = getch();
	}
}
