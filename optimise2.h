#ifndef OPTIMISE2_H
#define OPTIMISE2_H

#include "game.h"
#include "definitions.h"

// bap: Build Allocation Problem

struct bap_inputs {
    health_wide_t friendly_distribution[MODEL_CAPACITY];
    health_wide_t enemy_distribution[MODEL_CAPACITY];
    grid_wide_t capturables[CAPTURABLE_CAPACITY];
    gold_t budget;
};

typedef int16_t index_t;

void bap_A_cell_exists(const struct bap_inputs* const, const index_t i,
                       const index_t j);
void bap_A_row_exists(const struct bap_inputs* const, const index_t i);

#endif
