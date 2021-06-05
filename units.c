#include "units.h"
#include <assert.h>
#include <stddef.h>

void units_initialise(struct units* const units) {
    // Set counters
    units->start = 0;
    units->size = 0;
    // Setup indices
    for (unit_t i = 0; i < units_capacity; ++i)
        units->frees[i] = (i + 1) % units_capacity;

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

unit_t insert_with_frees(struct units* const units,
                         const struct unit* const unit) {
    assert(units->size <= units_capacity);
    // Check space to insert unit
    if (units->size == units_capacity)
        return null_unit;

    const unit_t index = units->start;
    units->start = units->frees[units->start];
    units->frees[index] = null_unit;
    ++units->size;
    units->data[index] = *unit;
    return index;
}

unit_t insert_with_players(struct units* const units,
                           const struct unit* const unit) {
    unit_t index = insert_with_frees(units, unit);
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

bool units_insert(struct units* const units, const struct unit* const unit) {
    assert(units->grid[unit->y][unit->x] == null_unit);
    assert(unit->player != null_player);

    const unit_t index = insert_with_players(units, unit);

    // insert_with_players may fail when units data structure is at capacity
    if (index == null_unit)
        return true;

    units->grid[unit->y][unit->x] = index;
    return false;
}

void delete_with_frees(struct units* const units, const unit_t index) {
    assert(units->size > 0);
    assert(units->frees[index] == null_unit);

    units->frees[index] = units->start;
    units->start = index;
    --units->size;
}

void delete_with_players(struct units* const units, const unit_t index) {
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

    delete_with_frees(units, index);
}

void units_delete(struct units* const units, const unit_t unit_index) {
    assert(unit_index != null_unit);
    delete_with_players(units, unit_index);
    const struct unit* const unit = &units->data[unit_index];
    units->grid[unit->y][unit->x] = null_unit;
}

void units_delete_at(struct units* const units, const grid_t x,
                     const grid_t y) {
    assert(units->grid[y][x] != null_unit);
    units_delete(units, units->grid[y][x]);
}

void units_move(struct units* const units, const unit_t unit, const grid_t x,
                const grid_t y) {
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

// Sets enabled property for each unit of a player
void units_set_enabled(struct units* const units, const player_t player,
                       const bool enabled) {
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

struct unit* units_get_at(struct units* const units, const grid_t x,
                          const grid_t y) {
    return units_get_by(units, units->grid[y][x]);
}

struct unit* units_get_by(struct units* const units, const unit_t unit) {
    assert(unit != null_unit);
    return &units->data[unit];
}

struct unit* units_get_by_safe(struct units* const units, const unit_t unit) {
    if (unit == null_unit)
        return NULL;

    return units_get_by(units, unit);
}

struct unit* units_get_first(struct units* const units, const player_t player) {
    return units_get_by_safe(units, units->firsts[player]);
}

unit_t index_by_pointer(const struct units* const units, const struct unit* const unit) {
    assert(unit - (struct unit*)&units->data != null_unit);
    return unit - (struct unit*)&units->data;
}

struct unit* units_get_next(struct units* const units, const struct unit* const unit) {
    assert (unit == NULL);
    const unit_t index = index_by_pointer(units, unit);
    return units_get_by_safe(units, units->nexts[index]);
}