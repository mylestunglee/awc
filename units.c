#include "units.h"
#include "constants.h"
#include "unit_constants.h"
#include <assert.h>
#include <stddef.h>

void units_initialise(struct units* const units) {
    // Set counters
    units->start = 0;
    units->size = 0;
    // Setup indices
    for (unit_t i = 0; i < UNITS_CAPACITY; ++i)
        units->frees[i] = (i + 1) % UNITS_CAPACITY;

    for (player_t i = 0; i < PLAYERS_CAPACITY; ++i)
        units->firsts[i] = NULL_UNIT;

    // Clear grid
    grid_t y = 0;
    do {
        grid_t x = 0;
        do {
            units->grid[y][x] = NULL_UNIT;
        } while (++x);
    } while (++y);

    units->selected = NULL_UNIT;
}

bool units_is_insertable(const struct units* const units) {
    return units->size < UNITS_CAPACITY;
}

unit_t insert_with_frees(struct units* const units,
                         const struct unit* const unit) {
    assert(units->size <= UNITS_CAPACITY);
    // Check space to insert unit
    if (units->size == UNITS_CAPACITY)
        return NULL_UNIT;

    const unit_t index = units->start;
    units->start = units->frees[units->start];
    units->frees[index] = NULL_UNIT;
    ++units->size;
    units->data[index] = *unit;
    return index;
}

unit_t insert_with_players(struct units* const units,
                           const struct unit* const unit) {
    unit_t index = insert_with_frees(units, unit);
    // Propagate failure
    if (index == NULL_UNIT)
        return NULL_UNIT;

    const player_t player = unit->player;

    // Link new node at front
    units->nexts[index] = units->firsts[player];
    units->prevs[index] = NULL_UNIT;

    // If not first unit of the player
    if (units->firsts[player] != NULL_UNIT)
        units->prevs[units->firsts[player]] = index;

    units->firsts[player] = index;
    return index;
}

void units_insert(struct units* const units, const struct unit* const unit) {
    assert(units->grid[unit->y][unit->x] == NULL_UNIT);
    assert(unit->player < PLAYERS_CAPACITY);
    assert(units_is_insertable(units));

    units->grid[unit->y][unit->x] = insert_with_players(units, unit);
}

void delete_with_frees(struct units* const units, const unit_t index) {
    assert(units->size > 0);
    assert(units->frees[index] == NULL_UNIT);

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
    if (units->nexts[index] != NULL_UNIT) {
        units->prevs[units->nexts[index]] = units->prevs[index];
    }

    delete_with_frees(units, index);
}

void units_delete(struct units* const units, const unit_t unit_index) {
    assert(unit_index != NULL_UNIT);
    delete_with_players(units, unit_index);
    const struct unit* const unit = &units->data[unit_index];
    units->grid[unit->y][unit->x] = NULL_UNIT;
}

void units_delete_at(struct units* const units, const grid_t x,
                     const grid_t y) {
    assert(units->grid[y][x] != NULL_UNIT);
    units_delete(units, units->grid[y][x]);
}

void units_delete_selected(struct units* const units) {
    units_delete(units, units->selected);
    units_clear_selection(units);
}

void units_move(struct units* const units, const unit_t unit, const grid_t x,
                const grid_t y) {
    const grid_t old_x = units->data[unit].x;
    const grid_t old_y = units->data[unit].y;

    // Do nothing if position has not changed
    if (old_x == x && old_y == y)
        return;

    assert(unit != NULL_UNIT);
    assert(units->grid[y][x] == NULL_UNIT);
    units->data[unit].x = x;
    units->data[unit].y = y;
    units->data[unit].capture_progress = 0;
    units->grid[old_y][old_x] = NULL_UNIT;
    units->grid[y][x] = unit;
}

void units_move_selection(struct units* const units, const grid_t x,
                          const grid_t y) {
    units_move(units, units->selected, x, y);
}

// Sets enabled property for each unit of a player
void units_set_enabled(struct units* const units, const player_t player,
                       const bool enabled) {
    assert(player < PLAYERS_CAPACITY);

    unit_t curr = units->firsts[player];

    while (curr != NULL_UNIT) {
        units->data[curr].enabled = enabled;
        curr = units->nexts[curr];
    }
}

// Clears all units of a player
void units_delete_player(struct units* const units, const player_t player) {
    assert(player < PLAYERS_CAPACITY);

    while (units->firsts[player] != NULL_UNIT)
        units_delete(units, units->firsts[player]);
}

struct unit* units_get_by_safe(struct units* const units, const unit_t unit) {
    if (unit == NULL_UNIT)
        return NULL;

    return units_get_by(units, unit);
}

struct unit* units_get_at(struct units* const units, const grid_t x,
                          const grid_t y) {
    return units_get_by_safe(units, units->grid[y][x]);
}

const struct unit* units_const_get_by(const struct units* const units,
                                      const unit_t unit) {
    assert(unit != NULL_UNIT);
    return &units->data[unit];
}

const struct unit* units_const_get_by_safe(const struct units* const units,
                                           const unit_t unit) {
    if (unit == NULL_UNIT)
        return NULL;

    return units_const_get_by(units, unit);
}

const struct unit* units_const_get_at(const struct units* const units,
                                      const grid_t x, const grid_t y) {
    return units_const_get_by_safe(units, units->grid[y][x]);
}

struct unit* units_get_by(struct units* const units, const unit_t unit) {
    assert(unit != NULL_UNIT);
    return &units->data[unit];
}

struct unit* units_get_first(struct units* const units, const player_t player) {
    return units_get_by_safe(units, units->firsts[player]);
}

unit_t index_by_pointer(const struct units* const units,
                        const struct unit* const unit) {
    assert(unit - (struct unit*)&units->data != NULL_UNIT);
    return unit - (struct unit*)&units->data;
}

struct unit* units_get_next(struct units* const units,
                            const struct unit* const unit) {
    assert(unit != NULL);
    const unit_t index = index_by_pointer(units, unit);
    return units_get_by_safe(units, units->nexts[index]);
}

const struct unit* units_const_get_first(const struct units* const units,
                                         const player_t player) {
    return units_const_get_by_safe(units, units->firsts[player]);
}

bool units_is_owner(const struct units* const units, const player_t player) {
    return units->firsts[player] != NULL_UNIT;
}

const struct unit* units_const_get_next(const struct units* const units,
                                        const struct unit* const unit) {
    assert(unit != NULL);
    const unit_t index = index_by_pointer(units, unit);
    return units_const_get_by_safe(units, units->nexts[index]);
}

const struct unit* units_const_get_next_cyclic(const struct units* const units,
                                               const struct unit* const unit) {
    const struct unit* const next = units_const_get_next(units, unit);
    if (next == NULL)
        return units_const_get_by(units, units->firsts[unit->player]);
    else
        return next;
}

struct unit* units_get_selected(struct units* const units) {
    return units_get_by(units, units->selected);
}

const struct unit* units_const_get_selected(const struct units* const units) {
    return units_const_get_by(units, units->selected);
}

void units_select_at(struct units* const units, const grid_t x,
                     const grid_t y) {
    assert(units->grid[y][x] != NULL_UNIT);
    units->selected = units->grid[y][x];
}

void units_clear_selection(struct units* const units) {
    units->selected = NULL_UNIT;
}

bool units_has_selection(const struct units* const units) {
    return units->selected != NULL_UNIT;
}

void units_disable_selection(struct units* const units) {
    units_get_by(units, units->selected)->enabled = false;
}

bool units_mergable(const struct unit* const source,
                    const struct unit* const target) {
    assert(source);
    assert(target);
    return source->player == target->player &&
           !(source->health == HEALTH_MAX && target->health == HEALTH_MAX) &&
           source->model == target->model;
}

health_t units_merge_health(const struct unit* const source,
                            const struct unit* const target) {
    assert(units_mergable(source, target));
    health_wide_t merged_health =
        (health_wide_t)source->health + (health_wide_t)target->health;
    if (merged_health > (health_wide_t)HEALTH_MAX)
        return HEALTH_MAX;
    else
        return merged_health;
}

bool units_exists(const struct units* const units, const grid_t x,
                  const grid_t y) {
    return units->grid[y][x] != NULL_UNIT;
}

bool units_direct(const model_t model) { return models_min_range[model] == 0; }

bool units_ranged(const model_t model) { return models_min_range[model] > 0; }

bool units_update_capture_progress(struct units* const units,
                                   const health_t progress) {
    struct unit* const selected = units_get_selected(units);

    assert(selected->capture_progress < CAPTURE_COMPLETION);
    selected->capture_progress += progress;
    if (selected->capture_progress >= CAPTURE_COMPLETION) {
        selected->capture_progress = 0;
        return true;
    }
    return false;
}