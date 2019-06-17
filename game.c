#include <stdio.h>
#include "console.h"
#include "game.h"
#include "graphics.h"

static void game_map_initialise(grid_index map[grid_size][grid_size]) {
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

static void game_labels_clear(uint8_t labels[grid_size][grid_size]) {
	grid_index y = 0;
	do {
		grid_index x = 0;
		do {
			labels[y][x] = 0;
		} while (++x);
	} while (++y);
}

static void game_labels_explore(const struct game* const game, uint8_t labels[grid_size][grid_size]) {
	uint8_t stack[10] = {0};
	uint8_t size = 0;

	stack[0] = 3;
	++size;
	uint8_t x = game->units.data[game->selected].x;
	uint8_t y = game->units.data[game->selected].y;

	uint8_t energy = 4;
	/*	dir = 0 => right
		dir = 1 => down
		dir = 2 => left
		dir = 3 => up
	*/

	labels[y][x] = energy;

	while (size > 0) {
		// print stack
		for (int i = 0; i < size; i++) {
			printf("%d", stack[i]);
		}
		printf("\n");
	
		// TODO
		uint8_t cost = 1;
		uint8_t top = stack[size - 1];
		if (energy <= cost ||
			(top == 0 && x == grid_max) ||
			(top == 1 && y == grid_max) ||
			(top == 2 && x == 0) ||
			(top == 3 && y == 0)) {
			// pop
			while (size > 0 && stack[size - 1] == 0) {
				--size;
				top = stack[size - 1];
				switch (top) {
					case 0: {
						--x;
						break;
					}
					case 1: {
						--y;
						break;
					}
					case 2: {
						++x;
						break;
					}
					case 3: {
						++y;
					}
				}
				
				energy += cost;
			}
			// rotate
			if (size > 0) {
				--stack[size - 1];
			}
			continue;
		}
		energy -= cost;
		// if current square has better energy then go pop
		switch (top) {
			case 0: {
				++x;
				break;
			}
			case 1: {
				++y;
				break;
			}
			case 2: {
				--x;
				break;
			}
			case 3: {
				--y;
			}
		}
		// label
		if (labels[y][x] < energy) {
			labels[y][x] = energy;
		}
		// next
		stack[size] = 3;
		++size;
	}
}

void game_initialise(struct game* const game) {
	// TODO: fix order
	game->x = 0;
	game->y = 0;
	game_map_initialise(game->map);
	game_labels_clear(game->labels);
	units_initialise(&game->units);
	game->selected = null_unit;
}

void game_loop(struct game* const game) {
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
				// Select unit
				if (unit != null_unit) {
					game->selected = unit;
					game_labels_explore(game, game->labels);
				}
			} else {
				// Cursor over selected unit
				if (game->selected == unit) {
					game->selected = null_unit;
					game_labels_clear(game->labels);
				} else if (game->labels[game->y][game->x] != 0) {
					units_move(&game->units, game->selected, game->x, game->y);
					game->selected = null_unit;
					game_labels_clear(game->labels);
				}
			}
		}

		render(game);

		printf("%u %u", game->x, game->y);

		input = getch();
	}
}
