#include <stdio.h>
#include "game.h"
#include "graphics.h"

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

	render(&game);

	reset_style();

	/*
	printf("\e[1;%umHello, world!\n", 31); // red
	printf("\e[1;%umHello, world!\n", 32); // green
	printf("\e[1;%umHello, world!\n", 33); // yellow
	printf("\e[1;%umHello, world!\n", 35); // blue
	*/

	return 0;
}
