#ifndef OPTIMISE2_H
#define OPTIMISE2_H

#include "definitions.h"
#include "game.h"
#include <glpk.h>

struct bap_inputs {
    health_wide_t friendly_distribution[MODEL_CAPACITY];
    health_wide_t enemy_distribution[MODEL_CAPACITY];
    grid_wide_t capturables[CAPTURABLE_CAPACITY];
    gold_t budget;
};

typedef int16_t index_t;

bool a_cell_exists(const struct bap_inputs* const, const index_t,
                   const index_t);
bool a_row_exists(const struct bap_inputs* const, const index_t);
bool a_column_exists(const struct bap_inputs* const, const index_t);
bool b_cell_exists(const struct bap_inputs* const, const index_t,
                   const index_t);
bool b_row_exists(const struct bap_inputs* const, const index_t);
bool b_column_exists(const struct bap_inputs* const, const index_t);
bool s_column_exists(const struct bap_inputs* const, const index_t);

struct bap_temps {
    glp_prob* const problem;
    index_t curr_index;
    index_t a_column_index;
    index_t b_column_index;
    index_t z_column_index;
    index_t a_row_index;
    index_t b_row_index;
    index_t s_row_index;
    index_t f_row_index;
};

#endif
