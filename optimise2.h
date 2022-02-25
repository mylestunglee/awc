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
bool a_i_j_exists(const struct bap_inputs* const, const index_t, const index_t);
bool a_i_exists(const struct bap_inputs* const, const index_t);
bool a_j_exists(const struct bap_inputs* const, const index_t);
bool b_i_j_exists(const struct bap_inputs* const, const index_t, const index_t);
bool b_i_exists(const struct bap_inputs* const, const index_t);
bool b_j_exists(const struct bap_inputs* const, const index_t);
bool allocation_exists(const struct bap_inputs* const, const index_t);
bool surplus_j_exists(const struct bap_inputs* const, const index_t);
bool problem_exists(const struct bap_inputs* const);

index_t count_distribution_rows(const struct bap_inputs* const);
index_t count_allocation_rows(const struct bap_inputs* const);
index_t count_budget_row(const struct bap_inputs* const);
index_t count_rows(const struct bap_inputs* const);
void set_next_row(struct bap_temps* const, const char variable, const index_t,
                  const int, const double, const double upper_bound);
void set_distribution_rows(const struct bap_inputs* const,
                           struct bap_temps* const);
void set_allocation_rows(const struct bap_inputs* const,
                         struct bap_temps* const);
void set_budget_row(const struct bap_inputs* const, struct bap_temps* const);
void set_surplus_rows(const struct bap_inputs* const, struct bap_temps* const);
void create_rows(const struct bap_inputs* const, struct bap_temps* const);

index_t count_a_columns(const struct bap_inputs* const);
index_t count_b_columns(const struct bap_inputs* const);
index_t count_objective_column(const struct bap_inputs* const);
index_t count_columns(const struct bap_inputs* const);
void set_next_matrix_column(struct bap_temps* const, const char, const index_t,
                            const index_t, const int, const double, const int);
void set_a_columns(const struct bap_inputs* const, struct bap_temps* const);
void set_b_columns(const struct bap_inputs* const, struct bap_temps* const);
void set_next_objective_column(struct bap_temps* const);
void set_columns(const struct bap_inputs* const, struct bap_temps* const);
void create_columns(const struct bap_inputs* const, struct bap_temps* const);

void sparse_matrix_set(struct bap_temps* const, const int, const int,
                       const double);
void set_distribution_submatrix(const struct bap_inputs* const,
                                struct bap_temps* const);
void set_allocation_submatrix(const struct bap_inputs* const,
                              struct bap_temps* const);
void set_budget_submatrix(const struct bap_inputs* const,
                          struct bap_temps* const);
double calc_surplus_submatrix_value(const struct bap_inputs* const,
                                    const index_t, const index_t);
void set_surplus_submatrix(const struct bap_inputs* const,
                           struct bap_temps* const);
void set_matrix(const struct bap_inputs* const, struct bap_temps* const);

#endif

#define SPARSE_MATRIX_LENGTH (MODEL_CAPACITY * MODEL_CAPACITY * MODEL_CAPACITY)

struct bap_temps {
    glp_prob* const problem;
    index_t curr_index;
    index_t a_column_start_index;
    index_t a_column_end_index;
    index_t b_column_start_index;
    index_t b_column_end_index;
    index_t z_column_index;
    index_t distribution_row_start_index;
    index_t distribution_row_end_index;
    index_t allocation_row_start_index; // start/end remove?
    index_t allocation_row_end_index;
    index_t budget_row_index;
    index_t surplus_row_start_index;
    index_t surplus_row_end_index;
    int matrix_rows[SPARSE_MATRIX_LENGTH];
    int matrix_columns[SPARSE_MATRIX_LENGTH];
    double matrix_values[SPARSE_MATRIX_LENGTH];
};

#endif
