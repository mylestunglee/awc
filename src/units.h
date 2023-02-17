#ifndef UNITS_H
#define UNITS_H

#include "definitions.h"
#include <stdbool.h>

#define HEALTH_MAX 100
#define UNITS_CAPACITY 255
#define NULL_UNIT UNITS_CAPACITY

typedef uint8_t model_t;
typedef uint8_t health_t;
typedef uint16_t health_wide_t;
typedef uint8_t capture_progress_t;
typedef uint8_t unit_t;

struct unit {
    grid_t x;
    grid_t y;
    player_t player;
    model_t model;
    health_t health;
    capture_progress_t capture_progress;
    bool enabled;
};

struct units {
    struct unit data[UNITS_CAPACITY];
    unit_t frees[UNITS_CAPACITY];
    unit_t firsts[PLAYERS_CAPACITY];
    unit_t prevs[UNITS_CAPACITY];
    unit_t nexts[UNITS_CAPACITY];
    unit_t grid[GRID_SIZE][GRID_SIZE];
    unit_t start;
    unit_t size;
    unit_t selected;
};

void units_initialise(struct units* const);
bool units_is_insertable(const struct units* const);
#ifdef EXPOSE_UNITS_INTERNALS
unit_t insert_with_frees(struct units* const, const struct unit* const);
unit_t insert_with_players(struct units* const, const struct unit* const);
#endif
void units_insert(struct units* const, const struct unit* const);
#ifdef EXPOSE_UNITS_INTERNALS
void delete_with_frees(struct units* const, const unit_t);
void delete_with_players(struct units* const, const unit_t);
void units_delete(struct units* const, const unit_t);
#endif
void units_delete_at(struct units* const, const grid_t, const grid_t);
void units_delete_selected(struct units* const);
#ifdef EXPOSE_UNITS_INTERNALS
void units_move(struct units* const, const unit_t, const grid_t, const grid_t);
#endif
void units_move_selection(struct units* const, const grid_t, const grid_t);
void units_set_enabled(struct units* const, const player_t, const bool);
void units_disable_non_turn(struct units* const, player_t);
void units_delete_player(struct units* const, const player_t);
#ifdef EXPOSE_UNITS_INTERNALS
struct unit* units_get_by(struct units* const, const unit_t);
struct unit* units_get_by_safe(struct units* const, const unit_t);
const struct unit* units_const_get_by(const struct units* const, const unit_t);
const struct unit* units_const_get_by_safe(const struct units* const,
                                           const unit_t);
#endif
bool units_exists_at(const struct units* const, const grid_t, const grid_t);
struct unit* units_get_at(struct units* const, const grid_t, const grid_t);
const struct unit* units_const_get_at(const struct units* const, const grid_t,
                                      const grid_t);
const struct unit* units_const_get_at_safe(const struct units* const,
                                           const grid_t, const grid_t);

struct unit* units_get_first(struct units* const, const player_t);
const struct unit* units_const_get_first(const struct units* const,
                                         const player_t);
#ifdef EXPOSE_UNITS_INTERNALS
unit_t index_by_pointer(const struct units* const, const struct unit* const);
struct unit* units_get_by_safe(struct units* const, const unit_t);
#endif
struct unit* units_get_next(struct units* const, const struct unit* const);
const struct unit* units_const_get_next(const struct units* const,
                                        const struct unit* const);
const struct unit* units_const_get_next_cyclic(const struct units* const,
                                               const struct unit* const);
bool units_is_owner(const struct units* const, const player_t);
struct unit* units_get_selected(struct units* const);
const struct unit* units_const_get_selected(const struct units* const);
void units_select_at(struct units* const, const grid_t, const grid_t);
void units_clear_selection(struct units* const);
bool units_has_selection(const struct units* const);
void units_disable_selection(struct units* const);
bool units_mergable(const struct unit* const, const struct unit* const);
health_t units_merge_health(const struct unit* const, const struct unit* const);
bool units_is_direct(const model_t);
bool units_is_ranged(const model_t);
bool units_update_capture_progress(struct units* const, const health_t);

#endif
