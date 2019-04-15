#include "game.h"
#include <stdio.h>

static void game_map_initialise(tile_index map[grid_size][grid_size]) {
	grid_index y = 0;
	do {
		grid_index x = 0;
		do {
			map[y][x] = 0;
		} while (++x);
	} while (++y);
}

void game_initialise(struct game* game) {
	game_map_initialise(game->map);
	units_initialise(&game->units);
}
