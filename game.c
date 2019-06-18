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

static void game_labels_explore_recursive(
	const struct game* const game,
	uint8_t labels[grid_size][grid_size],
	grid_index x,
	grid_index y,
	uint8_t energy) {
	if (energy <= 1) {
		return;
	}
	energy -= 1;
	if (labels[y][x] > energy) {
		return;
	}
	labels[y][x] = energy;
	game_labels_explore_recursive(game, labels, x + 1, y, energy);
	game_labels_explore_recursive(game, labels, x - 1, y, energy);
	game_labels_explore_recursive(game, labels, x, y + 1, energy);
	game_labels_explore_recursive(game, labels, x, y - 1, energy);
}

static void game_labels_explore(struct game* const game) {
	game_labels_explore_recursive(
		game,
		game->labels,
		game->units.data[game->selected].x,
		game->units.data[game->selected].y, 4);
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
					game_labels_explore(game);
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
