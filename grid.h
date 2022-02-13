#ifndef GRID_H
#define GRID_H

#include "game.h"

void grid_clear_uint8(uint8_t[GRID_SIZE][GRID_SIZE]);
void grid_clear_labels(struct game* const);
void grid_clear_territory(player_t[GRID_SIZE][GRID_SIZE]);
void grid_clear_player_territory(tile_t[GRID_SIZE][GRID_SIZE],
                                 player_t[GRID_SIZE][GRID_SIZE],
                                 const player_t);
void grid_correct_territory(player_t[GRID_SIZE][GRID_SIZE],
                            tile_t[GRID_SIZE][GRID_SIZE]);
void grid_compute_incomes(player_t[GRID_SIZE][GRID_SIZE],
                          gold_t[PLAYERS_CAPACITY]);
#ifdef expose_grid_internals
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
bool is_node_accessible(const struct game* const,
                        const struct list_node* const);
void explore_adjacent_tiles(struct game* const, const struct list_node* const,
                            const model_t);
void explore_node(struct game* const, const struct list_node* const,
                  const player_t, const model_t, const bool);
#endif
void grid_explore(struct game* const, const bool);
#ifdef expose_grid_internals
void clear_energies(energy_t[GRID_SIZE][GRID_SIZE]);
energy_t init_exploration_energy(const energy_t, const model_t);
#endif
void grid_explore_recursive(struct game* const, const bool, const energy_t);
void grid_find_path(struct game* const, grid_t x, grid_t y);

#endif
