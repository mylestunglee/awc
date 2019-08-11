#include <assert.h>
#include <stdio.h>
#include "units.h"

// Time complexity: O(units_capacity + players_capacity + grid_size^2)
void units_initialise(struct units* const units) {
	// Set counters
	units->start = 0;
	units->size = 0;
	// Setup indices
	for (unit_t i = 0; i < units_capacity; ++i)
		units->frees[i] = i;

	for (player_t i = 0; i < players_capacity; ++i)
		units->firsts[i] = null_unit;

	// Clear grid
	grid_t y = 0;
	do {
		grid_t x = 0;
		do {
			units->grid[y][x] = null_unit;
		} while (++x);
	} while (++y);
}

static unit_t units_frees_insert(struct units* const units, const struct unit* const unit) {
	// Check space to insert unit
	if (units->size == units_capacity)
		return null_unit;

	const unit_t index = units->start;
	units->data[index] = *unit;
	units->start = (units->start + 1) % units_capacity;
	++units->size;

	return index;
}

static unit_t units_players_insert(struct units* const units, const struct unit* const unit) {
	unit_t index = units_frees_insert(units, unit);
	// Propagate failure
	if (index == null_unit)
		return null_unit;

	const player_t player = unit->player;

	// Link new node at front
	units->nexts[index] = units->firsts[player];
	units->prevs[index] = null_unit;

	// If not first unit of the player
	if (units->firsts[player] != null_unit)
		units->prevs[units->firsts[player]] = index;

	units->firsts[player] = index;
	return index;
}

bool units_insert(struct units* const units, const struct unit unit) {
	assert(units->grid[unit.y][unit.x] == null_unit);

	const unit_t index = units_players_insert(units, &unit);

	// units_players_insert may fail when units data structure is at capacity
	if (index == null_unit)
		return true;

	units->grid[unit.y][unit.x] = index;
	return false;
}

static void units_frees_delete(struct units* const units, const unit_t index) {
	assert(units->size > 0);

	units->frees[(units_capacity + units->start - units->size) % units_capacity] = index;
	--units->size;
}

static void units_players_delete(struct units* const units, const unit_t index) {
	const player_t player = units->data[index].player;

	// Link firsts or prevs to skip over index
	if (units->firsts[player] == index) {
		units->firsts[player] = units->nexts[index];
	} else {
		units->nexts[units->prevs[index]] = units->nexts[index];
	}

	// Link next's prev to skip over index
	if (units->nexts[index] != null_unit) {
		units->prevs[units->nexts[index]] = units->prevs[index];
	}

	units_frees_delete(units, index);
}

void units_delete(struct units* const units, const unit_t unit_index) {
	assert(unit_index != null_unit);
	units_players_delete(units, unit_index);
	const struct unit* const unit = &units->data[unit_index];
	units->grid[unit->y][unit->x] = null_unit;
}

void units_move(struct units* const units, const unit_t unit, const grid_t x, const grid_t y) {
	const grid_t old_x = units->data[unit].x;
	const grid_t old_y = units->data[unit].y;

	// Do nothing if position has not changed
	if (old_x == x && old_y == y)
		return;

	assert(unit != null_unit);
	assert(units->grid[y][x] == null_unit);
	units->data[unit].x = x;
	units->data[unit].y = y;
	units->grid[old_y][old_x] = null_unit;
	units->grid[y][x] = unit;
}

// Prints queue of free indices
void units_frees_print(const struct units* const units) {
	assert(units_capacity > 0);

	const unit_t verboseness = 3;

	unit_t j = units->start;
	for (unit_t i = 0; i < units_capacity - units->size; ++i) {
		if (i < verboseness || i >= units_capacity - units->size - verboseness) {
			printf(unit_format" ", units->frees[j]);
		} else if (i == 3) {
			printf("... ");
		}

		j = (j + 1) % units_capacity;
	}
	printf("\n");
}

// Print linked lists of index and unit type pairs across all players
void units_players_print(const struct units* const units) {
	for (player_t player = 0; player < players_capacity; ++player) {
		unit_t curr = units->firsts[player];
		printf(player_format": ["unit_format"]", player, curr);
		while (curr != null_unit) {
			const unit_t next = units->nexts[curr];
			printf("->["unit_format": "unit_format" ("grid_format", "grid_format") "unit_format"]", curr, units->prevs[curr], units->data[curr].x, units->data[curr].y, next);
			curr = next;
		}
		printf("\n");
	}
}

// Prints mapping from grid to unit indices
void units_grid_print(const struct units* const units) {
	printf("{");
	grid_t y = 0;
	do {
		grid_t x = 0;
		do
		{
			if (units->grid[y][x] != null_unit)
				printf("("grid_format", "grid_format"): "unit_format" ", x, y, units->grid[y][x]);
		} while (++x);
	} while (++y);
	printf("}\n");
}

// Sets enabled property for each unit of a player
void units_set_enabled(struct units* const units, const player_t player, const bool enabled) {
	unit_t curr = units->firsts[player];

	while (curr != null_unit) {
		units->data[curr].enabled = enabled;
		curr = units->nexts[curr];
	}
}

// Clears all units of a player
void units_delete_player(struct units* const units, const player_t player) {
	while (units->firsts[player] != null_unit)
		units_delete(units, units->firsts[player]);
}
