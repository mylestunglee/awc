#ifndef optimise2_h
#define optimise2_h

#include "typedefs.h"
#include "units.h"

// bap: Build Allocation Problem

struct bap_inputs {
    health_wide_t friendly_distribution[model_capacity];
    health_wide_t enemy_distribution[model_capacity];
    tile_wide_t capturables[capturable_capacity];
    gold_t budget;
};

typedef int16_t index_t;

void bap_A_cell_exists(const struct bap_inputs* const, const index_t i,
                       const index_t j);
void bap_A_row_exists(const struct bap_inputs* const, const index_t i);

#endif
