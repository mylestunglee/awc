#include <assert.h>
#include <stdio.h>
#include "units.h"

// Get unit's model
unit_type unit_get_model(const struct unit* const unit) {
	return unit->type & unit_model_mask;
}

// Get unit's colour
unit_type unit_get_colour(const struct unit* const unit) {
	return unit->type >> unit_colour_offset;
}

// Time complexity: O(units_capacity + colours_capacity + grid_size^2)
void units_initialise(struct units* const units) {
	// Set counters
	units->start = 0;
	units->size = 0;
	// Setup indices
	for (unit_index i = 0; i < units_capacity; ++i)
		units->frees[i] = i;

	for (unit_type i = 0; i < colours_capacity; ++i)
		units->firsts[i] = null_unit;

	// Clear grid
	grid_index y = 0;
	do {
		grid_index x = 0;
		do {
			units->grid[y][x] = null_unit;
		} while (++x);
	} while (++y);
}

static unit_index units_frees_insert(struct units* const units, const struct unit* const unit) {
	// Check space to insert unit
	if (units->size == units_capacity)
		return null_unit;

	const unit_index index = units->start;
	units->units[index] = *unit;
	units->start = (units->start + 1) % units_capacity;
	++units->size;

	return index;
}

static unit_index units_colours_insert(struct units* const units, const struct unit* const unit) {
	unit_index index = units_frees_insert(units, unit);
	// Propagate failure
	if (index == null_unit)
		return null_unit;

	const unit_type colour = unit_get_colour(unit);

	// Link new node at front
	units->nexts[index] = units->firsts[colour];
	units->prevs[index] = null_unit;

	// If not first unit of the colour
	if (units->firsts[colour] != null_unit)
		units->prevs[units->firsts[colour]] = index;

	units->firsts[colour] = index;
	return index;
}

bool units_insert(struct units* const units, const struct unit unit) {
	if (units->grid[unit.y][unit.x] != null_unit)
		return true;

	const unit_index index = units_colours_insert(units, &unit);
	if (index == null_unit)
		return true;

	units->grid[unit.y][unit.x] = index;
	return false;
}

static void units_frees_delete(struct units* const units, const unit_index index) {
	assert(units->size > 0);

	units->frees[(units_capacity + units->start - units->size) % units_capacity] = index;
	--units->size;
}

static void units_colours_delete(struct units* const units, const unit_index index) {
	const unit_type colour = unit_get_colour(&units->units[index]);

	// Link firsts or prevs to skip over index
	if (units->firsts[colour] == index) {
		units->firsts[colour] = units->nexts[index];
	} else {
		units->nexts[units->prevs[index]] = units->nexts[index];
	}

	// Link next's prev to skip over index
	if (units->nexts[index] != null_unit) {
		units->prevs[units->nexts[index]] = units->prevs[index];
	}

	units_frees_delete(units, index);
}

void units_delete(struct units* const units, const grid_index x, const grid_index y) {
	assert(units->grid[y][x] != null_unit);
	units_colours_delete(units, units->grid[y][x]);
	units->grid[y][x] = null_unit;
}

// Prints queue of free indices
void units_frees_print(const struct units* const units) {
	assert(units_capacity > 0);

	unit_index j = units->start;
	for (unit_index i = 0; i < units_capacity - units->size; ++i) {
		if (i < verboseness || i >= units_capacity - units->size - verboseness) {
			printf(unit_index_format" ", units->frees[j]);
		} else if (i == 3) {
			printf("... ");
		}

		j = (j + 1) % units_capacity;
	}
	printf("\n");
}

// Print linked lists of index and unit type pairs across all colours
void units_colours_print(const struct units* const units) {
	for (unit_type colour = 0; colour < colours_capacity; ++colour) {
		unit_index curr = units->firsts[colour];
		printf(unit_type_format": ["unit_index_format"]", colour, curr);
		while (curr != null_unit) {
			const unit_index next = units->nexts[curr];
			printf("->["unit_type_format": "unit_type_format" ("grid_index_format", "grid_index_format") "unit_index_format"]", curr, units->prevs[curr], units->units[curr].x, units->units[curr].y, next);
			curr = next;
		}
		printf("\n");
	}
}

// Prints mapping from grid to unit indices
void units_grid_print(const struct units* const units) {
	printf("{");
	grid_index y = 0;
	do {
		grid_index x = 0;
		do
		{
			if (units->grid[y][x] != null_unit)
				printf("("grid_index_format", "grid_index_format"): "unit_index_format" ", x, y, units->grid[y][x]);
		} while (++x);
	} while (++y);
	printf("}\n");
}
