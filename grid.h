#ifndef grid_h
#define grid_h

#include "game.h"

void grid_clear_uint8(uint8_t[grid_size][grid_size]);
void grid_clear_energy(energy_t[grid_size][grid_size]);
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

#endif
