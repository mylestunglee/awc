#include <stdio.h>
#include "units.h"

int main() {
	struct units units;
	units_initialise(&units);

	units_frees_print(&units);
	units_colours_print(&units);
	units_grid_print(&units);
	printf("\n");

	units_insert(&units, (struct unit){.type = '\x64', .x = 2, .y = 1});
	units_insert(&units, (struct unit){.type = '\x63', .x = 1, .y = 3});
	units_insert(&units, (struct unit){.type = '\x65', .x = 0, .y = 3});

	units_frees_print(&units);
	units_colours_print(&units);
	units_grid_print(&units);
	printf("\n");

	units_delete(&units, 2, 1);

	units_frees_print(&units);
	units_colours_print(&units);
	units_grid_print(&units);

	return 0;
}
