#ifndef game_h
#define game_h

#include "units.h"

struct game {
	tile_index map[grid_size][grid_size];
	struct units units;
};

void game_initialise(struct game*);

#endif
