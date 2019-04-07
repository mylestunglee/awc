#include "units.h"
#include <stdio.h>
#include <assert.h>

void units_initialise(struct units* const units) {
	// Set counters
	units->start = 0;
	units->size = 0;
	// Setup indices
	for (unit_index i = 0; i < units_capacity; ++i)
		units->frees[i] = i;

	for (unit_type i = 0; i < max_colours; ++i)
		units->firsts[i] = units_capacity;

	// Clear grid
	grid_index y = 0;
	do {
		grid_index x = 0;
		do
		{
			units->grid[y][x] = units_capacity;
			++x;
		}
		while (x);
		++y;
	} while (y);
}

unit_index units_frees_insert(struct units* const units, const struct unit* const unit) {
	if (units->size == units_capacity)
		return units_capacity;

	const unit_index index = units->start;
	units->units[index] = *unit;
	units->start = (units->start + 1) % units_capacity;
	++units->size;

	return index;
}

// Get unit's colour
unit_type unit_get_colour(const struct unit* const unit) {
	return unit->type >> unit_colour_offset;
}

unit_index units_colours_insert(struct units* const units, const struct unit* const unit) {
	unit_index index = units_frees_insert(units, unit);
	// No space to insert unit
	if (index == units_capacity)
		// Propagate failure
		return units_capacity;

	const unit_type colour = unit_get_colour(unit);

	// Link new node at front
	units->nexts[index] = units->firsts[colour];
	units->prevs[index] = units_capacity;

	// If not first unit of the colour
	if (units->firsts[colour] != units_capacity)
		units->prevs[units->firsts[colour]] = index;

	units->firsts[colour] = index;
	return index;
}

bool units_insert(struct units* const units, const struct unit unit) {
	if (units->grid[unit.y][unit.x] != units_capacity)
		return true;

	const unit_index index = units_colours_insert(units, &unit);
	if (index == units_capacity)
		return true;

	units->grid[unit.y][unit.x] = index;
	return false;
}

void units_frees_delete(struct units* const units, const unit_index index) {
	assert(units->size > 0);

	units->frees[(units_capacity + units->start - units->size) % units_capacity] = index;
	--units->size;
}

void units_colours_delete(struct units* const units, const unit_index index) {
	const unit_type colour = unit_get_colour(&units->units[index]);

	// If deleting first node
	if (units->firsts[colour] == index) {
		units->firsts[colour] = units->nexts[index];
		if (units->firsts[colour] != units_capacity) {
			units->prevs[units->nexts[index]] = units_capacity;
		}
	} else {
		units->nexts[units->prevs[index]] = units->nexts[index];
		units->prevs[units->nexts[index]] = units->prevs[index];
	}

	units_frees_delete(units, index);
}

void units_delete(struct units* const units, const grid_index x, const grid_index y) {
	assert(units->grid[y][x] != units_capacity);
	units_colours_delete(units, units->grid[y][x]);
	units->grid[y][x] = units_capacity;
}

// Prints queue of free indices
void units_frees_print(const struct units* const units) {
	assert(units_capacity > 0);

	unit_index j = units->start;
	for (unit_index i = 0; i < units_capacity; ++i) {
		if (i < units_capacity - units->size)
			printf(unit_index_show_format" ", units->frees[j]);
		else
			printf(unit_index_hide_format" ");

		j = (j + 1) % units_capacity;
	}
	printf("\n");
}

// Print linked lists of index and unit type pairs across all colours
void units_colours_print(const struct units* const units) {
	for (unit_type colour = 0; colour < max_colours; ++colour) {
		unit_index curr = units->firsts[colour];
		if (curr != units_capacity) {
			printf(unit_type_format": ["unit_index_show_format"]", colour, curr);
			while (curr != units_capacity) {
				const unit_index next = units->nexts[curr];
				printf("->["unit_type_format" "unit_index_show_format"]", units->units[curr].type, next);
				curr = next;
			}
			printf("\n");
		}
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
			if (units->grid[y][x] != units_capacity)
				printf("("grid_index_format", "grid_index_format"): "unit_index_show_format" ", x, y, units->grid[y][x]);
			++x;
		} while (x);
		++y;
	} while (y);
	printf("}\n");
}

int main() {
	struct units units;
	units_initialise(&units);

	units_colours_print(&units);
	units_grid_print(&units);

	units_insert(&units, (struct unit){.type = '\x64', .x = 2, .y = 3});
	units_insert(&units, (struct unit){.type = '\xf4', .x = 1, .y = 3});
	units_insert(&units, (struct unit){.type = '\x65', .x = 0, .y = 3});

	units_colours_print(&units);
	units_grid_print(&units);

	units_delete(&units, 1, 3);

	units_colours_print(&units);
	units_grid_print(&units);

	return 0;
}
