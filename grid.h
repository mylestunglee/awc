#ifndef grid_h
#define grid_h

#include "game.h"

void grid_clear_all_uint8(uint8_t[grid_size][grid_size]);
void grid_clear_all_energy_t(uint16_t[grid_size][grid_size]);

void grid_explore(struct game* const);

#endif
