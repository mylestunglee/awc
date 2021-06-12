#ifndef grid_h
#define grid_h

#include "game.h"

void grid_clear_uint8(uint8_t[grid_size][grid_size]);
void grid_clear_territory(player_t[grid_size][grid_size]);
void grid_clear_player_territory(tile_t[grid_size][grid_size],
                                 player_t[grid_size][grid_size],
                                 const player_t);
void grid_correct(player_t[grid_size][grid_size], tile_t[grid_size][grid_size]);
void grid_compute_incomes(player_t[grid_size][grid_size],
                          gold_t[players_capacity]);
void grid_explore(struct game* const, const bool, const bool);
void grid_explore_recursive(struct game* const, const bool, const bool,
                            const energy_t);
void grid_find_path(struct game* const, grid_t x, grid_t y);
void grid_clear_labels(struct game* const);

#ifdef expose_grid_internals

void grid_clear_energy(energy_t[grid_size][grid_size]);
void grid_explore_mark_attackable_tile(struct game* const, const grid_t,
                                       const grid_t, const model_t,
                                       const player_t, const bool);

void grid_explore_mark_attackable_direct(struct game* const, const grid_t,
                                         const grid_t, const model_t,
                                         const player_t, const bool);

void grid_explore_mark_attackable_ranged(struct game* const, const grid_t,
                                         const grid_t, const model_t,
                                         const player_t, const bool);

bool is_node_unexplorable(const struct game* const,
                          const struct list_node* const, const player_t);

bool is_node_accessible(const struct game* const, const struct list_node* const);

void explore_adjacent_tiles(struct game* const, const struct list_node* const,
                            const movement_t);

void explore_node(struct game* const, const struct list_node* const,
                  const player_t, const model_t, const bool);

#endif

#endif
