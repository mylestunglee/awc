#ifndef units_h
#define units_h

#include <stdbool.h>
#include "definitions.h"

struct unit {
	health_t health;
	model_t model;
	player_t player;
	grid_t x;
	grid_t y;
	bool enabled;
};

struct units {
	struct unit data[units_capacity];
	unit_t start;
	unit_t size;
	unit_t frees[units_capacity];
	player_t firsts[players_capacity];
	unit_t prevs[units_capacity];
	unit_t nexts[units_capacity];
	unit_t grid[grid_size][grid_size];
};

void units_initialise(struct units* const);
bool units_insert(struct units* const, const struct unit);
void units_delete(struct units* const, const unit_t);
void units_move(struct units* const, const unit_t, const grid_t, const grid_t);
void units_frees_print(const struct units* const);
void units_players_print(const struct units* const);
void units_grid_print(const struct units* const);

#endif
