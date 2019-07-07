#ifndef game_h
#define game_h

#include "units.h"
#include "queue.h"

struct game {
	tile_index map[grid_size][grid_size];
	struct units units;
	grid_index x;
	grid_index y;
	unit_index selected;
	uint8_t labels[grid_size][grid_size];
	unit_energy workspace[grid_size][grid_size];
	struct queue queue;
	grid_index prev_x;
	grid_index prev_y;
};

void game_initialise(struct game* const);

void game_loop(struct game* const);

#endif
