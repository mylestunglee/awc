#include "bap_glpk_solver.h"
#include "unit_constants.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

#define FOR_MODEL(variable)                                                    \
    for (index_t variable = 0; variable < MODEL_CAPACITY; ++variable)
#define FOR_CAPTURABLE for (tile_t k = 0; k < CAPTURABLE_CAPACITY; ++k)
#define SYMBOLIC_NAME_LENGTH 16

bool a_i_j_exists(const struct bap_inputs* const inputs, const index_t i,
                  const index_t j) {
    return inputs->friendly_distribution[i] > 0 &&
           inputs->enemy_distribution[j] > 0 && model_damages[i][j] > 0;
}

bool a_i_exists(const struct bap_inputs* const inputs, const index_t i) {
    FOR_MODEL(j) if (a_i_j_exists(inputs, i, j)) return true;

    return false;
}

bool a_j_exists(const struct bap_inputs* const inputs, const index_t j) {
    FOR_MODEL(i) if (a_i_j_exists(inputs, i, j)) return true;

    return false;
}

bool allocation_exists(const struct bap_inputs* const inputs,
                       const index_t capturable) {
    return inputs->capturables[capturable] > 0;
}

bool b_i_j_exists(const struct bap_inputs* const inputs, const index_t i,
                  const index_t j) {
    FOR_CAPTURABLE
    if (capturable_buildable_models[k] <= i &&
        i < capturable_buildable_models[k + 1] &&
        inputs->enemy_distribution[j] > 0 && model_damages[i][j] > 0)
        return allocation_exists(inputs, k);

    return false;
}

bool b_i_exists(const struct bap_inputs* const inputs, const index_t i) {
    FOR_MODEL(j) if (b_i_j_exists(inputs, i, j)) return true;

    return false;
}

bool b_j_exists(const struct bap_inputs* const inputs, const index_t j) {
    FOR_MODEL(i) if (b_i_j_exists(inputs, i, j)) return true;

    return false;
}

bool surplus_j_exists(const struct bap_inputs* const inputs, const index_t j) {
    return a_j_exists(inputs, j) || b_j_exists(inputs, j);
}

bool bap_glpk_solvable(const struct bap_inputs* const inputs) {
    FOR_MODEL(i) if (b_i_exists(inputs, i)) return true;

    return false;
}

index_t count_distribution_rows(const struct bap_inputs* const inputs) {
    index_t count = 0;
    FOR_MODEL(i)
    if (a_i_exists(inputs, i))
        ++count;
    return count;
}

index_t count_allocation_rows(const struct bap_inputs* const inputs) {
    index_t count = 0;
    FOR_CAPTURABLE
    if (allocation_exists(inputs, k))
        ++count;
    return count;
}

index_t count_budget_row(void) { return 1; }

index_t count_surplus_rows(const struct bap_inputs* const inputs) {
    index_t count = 0;
    FOR_MODEL(j)
    if (surplus_j_exists(inputs, j))
        ++count;
    return count;
}

index_t count_rows(const struct bap_inputs* const inputs) {
    return count_distribution_rows(inputs) + count_allocation_rows(inputs) +
           count_budget_row() + count_surplus_rows(inputs);
}

void set_next_row(struct bap_glpk_temps* const temps, const char variable_name,
                  const index_t variable_index, const int bounds_type,
                  const double lower_bound, const double upper_bound) {
    assert('a' <= variable_name && variable_name < 'z');
    assert(lower_bound <= upper_bound);

    char name[SYMBOLIC_NAME_LENGTH];
    snprintf(name, sizeof name, "%c_%hu", variable_name, variable_index);
    glp_set_row_name(temps->problem, temps->curr_index, name);
    glp_set_row_bnds(temps->problem, temps->curr_index, bounds_type,
                     lower_bound, upper_bound);
    ++temps->curr_index;
}

void set_distribution_rows(const struct bap_inputs* const inputs,
                           struct bap_glpk_temps* const temps) {
    temps->distribution_row_start_index = temps->curr_index;
    FOR_MODEL(i)
    if (a_i_exists(inputs, i)) {
        const double units =
            (double)inputs->friendly_distribution[i] / (double)HEALTH_MAX;
        set_next_row(temps, 'd', i, GLP_FX, units, units);
    }
    temps->distribution_row_end_index = temps->curr_index;
}

void set_allocation_rows(const struct bap_inputs* const inputs,
                         struct bap_glpk_temps* const temps) {
    temps->allocation_row_index = temps->curr_index;
    FOR_CAPTURABLE
    if (allocation_exists(inputs, k))
        set_next_row(temps, 'a', k, GLP_UP, 0.0,
                     (double)inputs->capturables[k]);
}

void set_budget_row(const struct bap_inputs* const inputs,
                    struct bap_glpk_temps* const temps) {
    temps->budget_row_index = temps->curr_index;
    set_next_row(temps, 'b', 0, GLP_UP, 0.0, inputs->budget);
}

void set_surplus_rows(const struct bap_inputs* const inputs,
                      struct bap_glpk_temps* const temps) {
    temps->surplus_row_start_index = temps->curr_index;
    FOR_MODEL(j)
    if (surplus_j_exists(inputs, j))
        set_next_row(temps, 's', j, GLP_LO, 0.0, 0.0);
    temps->surplus_row_end_index = temps->curr_index;
}

void set_rows(const struct bap_inputs* const inputs,
              struct bap_glpk_temps* const temps) {
    temps->curr_index = 1;
    set_distribution_rows(inputs, temps);
    set_allocation_rows(inputs, temps);
    set_budget_row(inputs, temps);
    set_surplus_rows(inputs, temps);
}

void create_rows(const struct bap_inputs* const inputs,
                 struct bap_glpk_temps* const temps) {
    glp_add_rows(temps->problem, count_rows(inputs));
    set_rows(inputs, temps);
}

index_t count_a_columns(const struct bap_inputs* const inputs) {
    index_t count = 0;
    FOR_MODEL(i)
    FOR_MODEL(j)
    if (a_i_j_exists(inputs, i, j))
        ++count;
    return count;
}

index_t count_b_columns(const struct bap_inputs* const inputs) {
    index_t count = 0;
    FOR_MODEL(i)
    FOR_MODEL(j)
    if (b_i_j_exists(inputs, i, j))
        ++count;
    return count;
}

index_t count_objective_column(void) { return 1; }

index_t count_columns(const struct bap_inputs* const inputs) {
    return count_a_columns(inputs) + count_b_columns(inputs) +
           count_objective_column();
}

void set_next_matrix_column(struct bap_glpk_temps* const temps,
                            const char variable_name, const index_t i,
                            const index_t j, const int bounds_type,
                            const double upper_bound, const int variable_type) {
    assert('A' <= variable_name && variable_name <= 'Z');
    assert(0.0 <= upper_bound);

    char name[SYMBOLIC_NAME_LENGTH];
    snprintf(name, sizeof name, "%c_%hu_%hu", variable_name, i, j);
    glp_set_col_name(temps->problem, temps->curr_index, name);
    glp_set_col_bnds(temps->problem, temps->curr_index, bounds_type, 0.0,
                     upper_bound);
    glp_set_col_kind(temps->problem, temps->curr_index, variable_type);
    ++temps->curr_index;
}

void set_a_columns(const struct bap_inputs* const inputs,
                   struct bap_glpk_temps* const temps) {
    temps->a_column_start_index = temps->curr_index;
    FOR_MODEL(i)
    FOR_MODEL(j)
    if (a_i_j_exists(inputs, i, j))
        set_next_matrix_column(temps, 'A', i, j, GLP_DB,
                               (double)inputs->friendly_distribution[i] /
                                   (double)HEALTH_MAX,
                               GLP_CV);
    temps->a_column_end_index = temps->curr_index;
}

void set_b_columns(const struct bap_inputs* const inputs,
                   struct bap_glpk_temps* const temps) {
    temps->b_column_index = temps->curr_index;
    FOR_MODEL(i)
    FOR_MODEL(j)
    if (b_i_j_exists(inputs, i, j))
        set_next_matrix_column(temps, 'B', i, j, GLP_LO, 0.0, GLP_IV);
}

void set_z_column(struct bap_glpk_temps* const temps) {
    temps->z_column_index = temps->curr_index;
    glp_set_col_name(temps->problem, temps->curr_index, "z");
    glp_set_obj_coef(temps->problem, temps->curr_index, -1.0);
    glp_set_col_bnds(temps->problem, temps->curr_index, GLP_FR, 0.0, 0.0);
    ++temps->curr_index;
}

void set_columns(const struct bap_inputs* const inputs,
                 struct bap_glpk_temps* const temps) {
    temps->curr_index = 1;
    set_a_columns(inputs, temps);
    set_b_columns(inputs, temps);
    set_z_column(temps);
}

void create_columns(const struct bap_inputs* const inputs,
                    struct bap_glpk_temps* const temps) {
    glp_add_cols(temps->problem, count_columns(inputs));
    set_columns(inputs, temps);
}

void sparse_matrix_set(struct bap_glpk_temps* const temps, const int row,
                       const int column, const double value) {
    assert(temps->curr_index < SPARSE_MATRIX_LENGTH);
    temps->matrix_rows[temps->curr_index] = row;
    temps->matrix_columns[temps->curr_index] = column;
    temps->matrix_values[temps->curr_index] = value;
    ++temps->curr_index;
}

void set_distribution_submatrix(struct bap_glpk_temps* const temps) {
    for (index_t row = temps->distribution_row_start_index;
         row < temps->distribution_row_end_index; ++row)
        for (index_t column = temps->a_column_start_index;
             column < temps->a_column_end_index; ++column) {
            sparse_matrix_set(temps, row, column, 1.0);
        }
}

void set_allocation_submatrix(const struct bap_inputs* const inputs,
                              struct bap_glpk_temps* const temps) {
    index_t row = temps->allocation_row_index;
    FOR_CAPTURABLE
    if (allocation_exists(inputs, k)) {
        index_t column = temps->b_column_index;
        for (index_t i = capturable_buildable_models[k];
             i < capturable_buildable_models[k + 1]; ++i)
            FOR_MODEL(j)
        if (b_i_j_exists(inputs, i, j)) {
            sparse_matrix_set(temps, row, column, 1.0);
            ++column;
        }
        ++row;
    }
}

void set_budget_submatrix(const struct bap_inputs* const inputs,
                          struct bap_glpk_temps* const temps) {
    index_t column = temps->b_column_index;
    FOR_MODEL(i)
    FOR_MODEL(j)
    if (b_i_j_exists(inputs, i, j)) {
        sparse_matrix_set(temps, temps->budget_row_index, column,
                          model_costs[i]);
        ++column;
    }
}

double calc_surplus_submatrix_value(const struct bap_inputs* const inputs,
                                    const index_t i, const index_t j) {
    return (double)model_damages[i][j] * (units_is_ranged(i) ? 0.5 : 1.0) /
           (double)inputs->enemy_distribution[j];
}

void set_surplus_submatrix(const struct bap_inputs* const inputs,
                           struct bap_glpk_temps* const temps) {
    index_t a_column = temps->a_column_start_index;
    index_t b_column = temps->b_column_index;
    FOR_MODEL(i) {
        index_t row = temps->surplus_row_start_index;
        FOR_MODEL(j) {
            if (a_i_j_exists(inputs, i, j)) {
                sparse_matrix_set(temps, row, a_column,
                                  calc_surplus_submatrix_value(inputs, i, j));
                ++a_column;
            }
            if (b_i_j_exists(inputs, i, j)) {
                sparse_matrix_set(temps, row, b_column,
                                  calc_surplus_submatrix_value(inputs, i, j));
                ++b_column;
            }
            if (surplus_j_exists(inputs, j)) {
                ++row;
            }
        }
    }

    for (index_t row = temps->surplus_row_start_index;
         row < temps->surplus_row_end_index; ++row)
        sparse_matrix_set(temps, row, temps->z_column_index, -1.0);
}

void set_matrix(const struct bap_inputs* const inputs,
                struct bap_glpk_temps* const temps) {
    temps->curr_index = 1;
    set_distribution_submatrix(temps);
    set_allocation_submatrix(inputs, temps);
    set_budget_submatrix(inputs, temps);
    set_surplus_submatrix(inputs, temps);
    glp_load_matrix(temps->problem, temps->curr_index - 1, temps->matrix_rows,
                    temps->matrix_columns, temps->matrix_values);
}

void bap_glpk_temps_preinitialise(struct bap_glpk_temps* const temps) {
    temps->problem = glp_create_prob();
    temps->matrix_rows[0] = 0;
    temps->matrix_columns[0] = 0;
    temps->matrix_values[0] = 0.0;
}

void bap_glpk_temps_initialise(const struct bap_inputs* const inputs,
                               struct bap_glpk_temps* const temps) {
    create_rows(inputs, temps);
    create_columns(inputs, temps);
    set_matrix(inputs, temps);
}

int glpk_solve(struct bap_glpk_temps* const temps) {
    glp_iocp parameters;
    glp_init_iocp(&parameters);
    parameters.presolve = GLP_ON;
    parameters.msg_lev = GLP_MSG_ERR;
    return glp_intopt(temps->problem, &parameters);
}

void parse_results(const struct bap_inputs* const inputs,
                   const struct bap_glpk_temps* const temps,
                   grid_wide_t outputs[MODEL_CAPACITY]) {
    memset(outputs, 0, sizeof(grid_wide_t) * MODEL_CAPACITY);
    index_t column = temps->a_column_start_index;

    FOR_MODEL(i)
    FOR_MODEL(j)
    if (b_i_j_exists(inputs, i, j)) {
        outputs[i] += glp_mip_col_val(temps->problem, column);
        ++column;
    }
}

int bap_glpk_solve(const struct bap_inputs* const inputs,
                   grid_wide_t outputs[MODEL_CAPACITY], void* const workspace) {
    assert(sizeof(struct bap_glpk_temps) <= sizeof(struct list));
    struct bap_glpk_temps* const temps = (struct bap_glpk_temps*)workspace;
    bap_glpk_temps_preinitialise(temps);
    bap_glpk_temps_initialise(inputs, temps);
    const int error = glpk_solve(temps);
    if (!error)
        parse_results(inputs, temps, outputs);

    glp_delete_prob(temps->problem);

    return error;
}
