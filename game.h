#ifndef game_h
#define game_h

#include "units.h"
#include "queue.h"

struct game {
	tile_t map[grid_size][grid_size];
	struct units units;
	grid_t x;
	grid_t y;
	unit_t selected;
	uint8_t labels[grid_size][grid_size];
	energy_t workspace[grid_size][grid_size];
	struct queue queue;
	grid_t prev_x;
	grid_t prev_y;
};

void game_initialise(struct game* const);

void game_loop(struct game* const);

#endif
