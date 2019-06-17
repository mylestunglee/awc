#ifndef units_h
#define units_h

#include <stdbool.h>
#include "definitions.h"

struct unit {
	unit_health health;
	unit_type type;
	grid_index x;
	grid_index y;
};

struct units {
	struct unit data[units_capacity];
	unit_index start;
	unit_index size;
	unit_index frees[units_capacity];
	unit_index firsts[players_capacity];
	unit_index prevs[units_capacity];
	unit_index nexts[units_capacity];
	unit_index grid[grid_size][grid_size];
};

unit_type unit_get_model(const struct unit* const);
unit_type unit_get_player(const struct unit* const);

void units_initialise(struct units* const);
bool units_insert(struct units* const, const struct unit);
void units_delete(struct units* const, const grid_index, const grid_index);
void units_move(struct units* const, const unit_index, const grid_index, const grid_index);
void units_frees_print(const struct units* const);
void units_players_print(const struct units* const);
void units_grid_print(const struct units* const);

#endif
