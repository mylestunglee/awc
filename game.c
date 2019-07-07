#include <stdio.h>
#include "console.h"
#include "game.h"
#include "graphics.h"
#include "grid.h"

static void game_map_initialise(tile_index map[grid_size][grid_size]) {
	grid_index y = 0;
	do {
		grid_index x = 0;
		do {
			map[y][x] = 0;
		} while (++x);
	} while (++y);

	for (int i = 0; i < 20; i++) {
		for (int j = 0; j < 6; j++) {
			if (i < 9) {
				map[j][i] = 1;
			} else {
				map[j][i] = 5;
			}
		}
	}

	for (int i = 0; i < grid_capacity; i++) {
		map[4][i] = i;
	}

	map[4][10] = 1;
	map[5][10] = 1;
}

void game_initialise(struct game* const game) {
	// TODO: fix order
	game->x = 0;
	game->y = 0;
	game_map_initialise(game->map);
	grid_clear_all_uint8(game->labels);
	grid_clear_all_unit_energy(game->workspace);
	units_initialise(&game->units);
	game->selected = null_unit;
	queue_initialise(&game->queue);
}

void game_loop(struct game* const game) {
	render(game);

	char input = getch();
	grid_index prev_x = game->x;
	grid_index prev_y = game->y;

	while (input != 'q') {
		if (input == 'a') {
			prev_x = game->x;
			prev_y = game->y;
			--game->x;
		}
		else if (input == 'd') {
			prev_x = game->x;
			prev_y = game->y;
			++game->x;
		}
		else if (input == 'w') {
			prev_x = game->x;
			prev_y = game->y;
			--game->y;
		}
		else if (input == 's') {
			prev_x = game->x;
			prev_y = game->y;
			++game->y;
		}

		if (input == ' ') {
			const unit_index unit = game->units.grid[game->y][game->x];
			if (game->selected == null_unit) {
				// Select unit
				if (unit != null_unit) {
					game->selected = unit;
					grid_explore(game);
					grid_clear_all_unit_energy(game->workspace);
				}
			} else {
				// Cursor over selected unit
				if (game->selected == unit) {
					game->selected = null_unit;
					grid_clear_all_uint8(game->labels);
				// Move to accessible tile
				} else if ((game->labels[game->y][game->x] & accessible_bit) != 0) {
					units_move(&game->units, game->selected, game->x, game->y);
					game->selected = null_unit;
					grid_clear_all_uint8(game->labels);
				// Attack enemy unit
				} else if (
					(game->labels[game->y][game->x] & attackable_bit) != 0 &&
					(game->labels[prev_y][prev_x] & accessible_bit) != 0 &&
					game->units.data[unit].player != game->units.data[game->selected].player) {

					units_move(&game->units, game->selected, prev_x, prev_y);

					//game->units.data[unit].health = 0;
					units_delete(&game->units, game->x, game->y);

					game->selected = null_unit;
					grid_clear_all_uint8(game->labels);

				}

			}
		}


		render(game);

		//printf("%u %u %s", game->x, game->y, grid_names[game->map[game->y][game->x]]);
		printf("%u %u", game->x, game->y);

		input = getch();
	}
}
