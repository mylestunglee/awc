#ifndef BAP_H
#define BAP_H

#include "definitions.h"
#include "game.h"

struct bap_inputs {
    health_wide_t friendly_distribution[MODEL_CAPACITY];
    health_wide_t enemy_distribution[MODEL_CAPACITY];
    grid_wide_t capturables[CAPTURABLE_CAPACITY];
    gold_t budget;
};

void bap_solve(const struct bap_inputs* const, grid_wide_t[MODEL_CAPACITY],
               void* const);

#endif