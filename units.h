#ifndef units_h
#define units_h

#include <stdbool.h>
#include "definitions.h"

struct unit;
struct units;

struct unit {
	unit_health health;
	unit_type type;
	grid_index x;
	grid_index y;
};

struct units {
	struct unit units[units_capacity];
	unit_index start;
	unit_index size;
	unit_index frees[units_capacity];
	unit_index firsts[max_colours];
	unit_index prevs[units_capacity];
	unit_index nexts[units_capacity];
	unit_index grid[grid_size][grid_size];
};

void units_initialise(struct units* const);

unit_index units_frees_insert(struct units* const, const struct unit* const);
unit_type unit_get_colour(const struct unit* const);
unit_index units_colours_insert(struct units* const, const struct unit* const);
bool units_insert(struct units* const, const struct unit);

void units_frees_delete(struct units* const, const unit_index);
void units_colours_delete(struct units* const, const unit_index);
void units_delete(struct units* const, const grid_index, const grid_index);

void units_frees_print(const struct units* const);
void units_colours_print(const struct units* const);
void units_grid_print(const struct units* const);

#endif
