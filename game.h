#ifndef game_h
#define game_h

#include "units.h"

struct game {
	grid_index map[grid_size][grid_size];
	struct units units;
	grid_index x;
	grid_index y;
	unit_index selected;
	uint8_t labels[grid_size][grid_size];
};

void game_initialise(struct game*);

void game_loop(struct game*);

#endif
