#ifndef BAP_GLPK_SOLVER_H
#define BAP_GLPK_SOLVER_H

#include "bap.h"
#include <glpk.h>

typedef int16_t index_t;

#define SPARSE_MATRIX_LENGTH                                                   \
    (MODEL_CAPACITY +                                                          \
     (CAPTURABLE_CAPACITY + 1) * MODEL_CAPACITY * MODEL_CAPACITY +             \
     3 * MODEL_CAPACITY * MODEL_CAPACITY * MODEL_CAPACITY)

struct bap_temps {
    glp_prob* problem;
    index_t curr_index;
    index_t a_column_start_index;
    index_t a_column_end_index;
    index_t b_column_index;
    index_t z_column_index;
    index_t distribution_row_start_index;
    index_t distribution_row_end_index;
    index_t allocation_row_index;
    index_t budget_row_index;
    index_t surplus_row_start_index;
    index_t surplus_row_end_index;
    int matrix_rows[SPARSE_MATRIX_LENGTH];
    int matrix_columns[SPARSE_MATRIX_LENGTH];
    double matrix_values[SPARSE_MATRIX_LENGTH];
};

#ifdef EXPOSE_BAP_GLPK_SOLVER_INTERNALS
bool a_i_j_exists(const struct bap_inputs* const, const index_t, const index_t);
bool a_i_exists(const struct bap_inputs* const, const index_t);
bool a_j_exists(const struct bap_inputs* const, const index_t);
bool allocation_exists(const struct bap_inputs* const, const index_t);
bool b_i_j_exists(const struct bap_inputs* const, const index_t, const index_t);
bool b_i_exists(const struct bap_inputs* const, const index_t);
bool b_j_exists(const struct bap_inputs* const, const index_t);
bool surplus_j_exists(const struct bap_inputs* const, const index_t);
#endif
bool bap_glpk_solvable(const struct bap_inputs* const);

#ifdef EXPOSE_BAP_GLPK_SOLVER_INTERNALS
index_t count_distribution_rows(const struct bap_inputs* const);
index_t count_allocation_rows(const struct bap_inputs* const);
index_t count_budget_row(const struct bap_inputs* const);
index_t count_surplus_rows(const struct bap_inputs* const);
index_t count_rows(const struct bap_inputs* const);
void set_next_row(struct bap_glpk_temps* const, const char variable,
                  const index_t, const int, const double,
                  const double upper_bound);
void set_distribution_rows(const struct bap_inputs* const,
                           struct bap_glpk_temps* const);
void set_allocation_rows(const struct bap_inputs* const,
                         struct bap_glpk_temps* const);
void set_budget_row(const struct bap_inputs* const,
                    struct bap_glpk_temps* const);
void set_surplus_rows(const struct bap_inputs* const,
                      struct bap_glpk_temps* const);
void create_rows(const struct bap_inputs* const, struct bap_glpk_temps* const);

index_t count_a_columns(const struct bap_inputs* const);
index_t count_b_columns(const struct bap_inputs* const);
index_t count_objective_column(const struct bap_inputs* const);
index_t count_columns(const struct bap_inputs* const);
void set_next_matrix_column(struct bap_glpk_temps* const, const char,
                            const index_t, const index_t, const int,
                            const double, const int);
void set_a_columns(const struct bap_inputs* const,
                   struct bap_glpk_temps* const);
void set_b_columns(const struct bap_inputs* const,
                   struct bap_glpk_temps* const);
void set_z_column(struct bap_glpk_temps* const);
void set_columns(const struct bap_inputs* const, struct bap_glpk_temps* const);
void create_columns(const struct bap_inputs* const,
                    struct bap_glpk_temps* const);

void sparse_matrix_set(struct bap_glpk_temps* const, const int, const int,
                       const double);
void set_distribution_submatrix(const struct bap_inputs* const,
                                struct bap_glpk_temps* const);
void set_allocation_submatrix(const struct bap_inputs* const,
                              struct bap_glpk_temps* const);
void set_budget_submatrix(const struct bap_inputs* const,
                          struct bap_glpk_temps* const);
double calc_surplus_submatrix_value(const struct bap_inputs* const,
                                    const index_t, const index_t);
void set_surplus_submatrix(const struct bap_inputs* const,
                           struct bap_glpk_temps* const);
void set_matrix(const struct bap_inputs* const, struct bap_glpk_temps* const);

void initialise_bap(const struct bap_inputs* const, struct bap_temps* const);
int glpk_solve(struct bap_temps* const);
void parse_results(const struct bap_inputs* const,
                   const struct bap_temps* const, grid_wide_t[MODEL_CAPACITY]);
#endif
int bap_glpk_solve(const struct bap_inputs* const, grid_wide_t[MODEL_CAPACITY],
                   void* const);

#endif