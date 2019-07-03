#include <stdio.h>
#include "game.h"
#include "graphics.h"
#include "queue.h"

int main() {
	struct game game;
	game_initialise(&game);
	/*
	struct units* units = &game.units;
	units_initialise(units);

	units_frees_print(units);
	units_colours_print(units);
	units_grid_print(units);
	printf("\n");

	units_insert(units, (struct unit){.type = '\x64', .x = 2, .y = 1});
	units_insert(units, (struct unit){.type = '\x63', .x = 1, .y = 3});
	units_insert(units, (struct unit){.type = '\x65', .x = 0, .y = 3});

	units_frees_print(units);
	units_colours_print(units);
	units_grid_print(units);
	printf("\n");

	units_delete(units, 2, 1);

	units_frees_print(units);
	units_colours_print(units);
	units_grid_print(units);
	*/

	/*
	struct path path;
	path_initialise(&path);
	path_push(&path);
	path_push(&path);
	while (path.size > 0) {
		path_print(&path);
		path_next(&path);
	}
	path_print(&path);

	return 0;
	*/

	units_insert(&game.units, (struct unit){.type = '\x20', .x = 3, .y = 3});

	game_loop(&game);

	reset_style();

	return 0;
}
