#ifndef units_h
#define units_h

#include "typedefs.h"
#include <stdbool.h>

#define health_max (health_t)0xff
#define units_capacity (unit_t)0xff

typedef uint8_t unit_t;
typedef uint8_t health_t;
typedef uint16_t health_wide_t;
typedef uint8_t model_t;
typedef uint16_t capture_progress_t;

struct unit {
    health_t health;
    model_t model;
    player_t player;
    grid_t x;
    grid_t y;
    bool enabled;
    capture_progress_t capture_progress;
};

struct units {
    struct unit data[units_capacity];
    unit_t start;
    unit_t size;
    unit_t frees[units_capacity];
    unit_t firsts[players_capacity];
    unit_t prevs[units_capacity];
    unit_t nexts[units_capacity];
    unit_t grid[grid_size][grid_size];
    unit_t selected;
};

void units_initialise(struct units* const);
bool units_insert(struct units* const, const struct unit* const);
void units_delete_at(struct units* const, const grid_t, const grid_t);
void units_delete_selected(struct units* const);
void units_move_selection(struct units* const, const grid_t, const grid_t);
void units_set_enabled(struct units* const, const player_t, const bool);
void units_delete_player(struct units* const, const player_t);
struct unit* units_get_at(struct units* const, const grid_t, const grid_t);
const struct unit* units_const_get_at(const struct units* const, const grid_t,
                                      const grid_t);
struct unit* units_get_by(struct units* const, const unit_t);
struct unit* units_get_first(struct units* const, const player_t);
struct unit* units_get_next(struct units* const, const struct unit* const);
const struct unit* units_const_get_first(const struct units* const,
                                         const player_t);
bool units_is_owner(const struct units* const, const player_t);
const struct unit* units_const_get_next(const struct units* const,
                                        const struct unit* const);
const struct unit* units_const_get_next_cyclic(const struct units* const,
                                               const struct unit* const);
const struct unit* units_const_get_by(const struct units* const, const unit_t);
struct unit* units_get_selected(struct units* const);
const struct unit* units_const_get_selected(const struct units* const);
void units_select_at(struct units* const, const grid_t, const grid_t);
void units_clear_selection(struct units* const);
bool units_has_selection(const struct units* const);
void units_disable_selection(struct units* const);
bool units_mergable(const struct unit* const, const struct unit* const);
bool units_exists(const struct units* const, const grid_t, const grid_t);
bool units_ranged(const model_t);
bool units_update_capture_progress(struct units* const, const health_t);

#ifdef expose_units_internals

void units_delete(struct units* const, const unit_t);
void units_move(struct units* const, const unit_t, const grid_t, const grid_t);
unit_t insert_with_frees(struct units* const, const struct unit* const);
unit_t insert_with_players(struct units* const, const struct unit* const);
void delete_with_frees(struct units* const, const unit_t);
void delete_with_players(struct units* const, const unit_t);

unit_t index_by_pointer(const struct units* const, const struct unit* const);
struct unit* units_get_by_safe(struct units* const, const unit_t);
const struct unit* units_const_get_by_safe(const struct units* const,
                                           const unit_t);
#endif

#endif
