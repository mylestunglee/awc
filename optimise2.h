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

#ifdef expose_optimise_internals
bool a_cell_exists(const struct bap_inputs* const, const index_t,
                   const index_t);
bool a_row_exists(const struct bap_inputs* const, const index_t);
bool a_column_exists(const struct bap_inputs* const, const index_t);
bool b_cell_exists(const struct bap_inputs* const, const index_t,
                   const index_t);
bool b_row_exists(const struct bap_inputs* const, const index_t);
bool b_column_exists(const struct bap_inputs* const, const index_t);
bool s_column_exists(const struct bap_inputs* const, const index_t);
bool problem_exists(const struct bap_inputs* const);

void create_row(struct bap_temps* const, const char variable, const index_t,
                const int, const double, const double upper_bound);
void create_distribution_rows(const struct bap_inputs* const,
                              struct bap_temps* const);
void create_allocation_rows(const struct bap_inputs* const,
                            struct bap_temps* const);
void create_budget_row(const struct bap_inputs* const, struct bap_temps* const);
void create_surplus_rows(const struct bap_inputs* const,
                         struct bap_temps* const);
void create_rows(const struct bap_inputs* const, struct bap_temps* const);

#endif

#define SPARSE_MATRIX_LENGTH (MODEL_CAPACITY * MODEL_CAPACITY * MODEL_CAPACITY)

struct bap_temps {
    glp_prob* const problem;
    index_t curr_index;
    index_t a_column_index;
    index_t b_column_index;
    index_t z_column_index;
    index_t distribution_row_index;
    index_t allocation_row_index;
    index_t budget_row_index;
    index_t surplus_row_index;
    int matrix_rows[SPARSE_MATRIX_LENGTH];
    int matrix_columns[SPARSE_MATRIX_LENGTH];
    double matrix_values[SPARSE_MATRIX_LENGTH];
};

#endif
