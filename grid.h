#ifndef grid_h
#define grid_h

#include "game.h"

void grid_clear_all_uint8(uint8_t[grid_size][grid_size]);
void grid_clear_all_energy_t(uint16_t[grid_size][grid_size]);
void grid_clear_territory(player_t[grid_size][grid_size]);
void grid_clear_player_territory(player_t[grid_size][grid_size], const player_t);
void grid_correct_map(player_t[grid_size][grid_size], tile_t[grid_size][grid_size]);

void grid_explore(struct game* const);

#endif
