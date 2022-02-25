#include "optimise2.h"
#include "unit_constants.h"
#include <assert.h>
#include <stdio.h>

#define FOR(variable)                                                          \
    for (index_t variable = 0; variable < MODEL_CAPACITY; ++variable)

bool a_i_j_exists(const struct bap_inputs* const inputs, const index_t i,
                  const index_t j) {
    return inputs->friendly_distribution[i] > 0 &&
           inputs->enemy_distribution[j] > 0 && units_damage[j][i] > 0;
}

bool a_i_exists(const struct bap_inputs* const inputs, const index_t i) {
    FOR(j) if (!a_i_j_exists(inputs, i, j)) return false;

    return true;
}

bool a_j_exists(const struct bap_inputs* const inputs, const index_t j) {
    FOR(i) if (!a_i_j_exists(inputs, i, j)) return false;

    return true;
}

bool b_i_j_exists(const struct bap_inputs* const inputs, const index_t i,
                  const index_t j) {
    FOR(i) if (!a_i_j_exists(inputs, i, j)) return false;

    return true;
}

bool b_i_exists(const struct bap_inputs* const inputs, const index_t i) {
    FOR(j) if (!b_i_j_exists(inputs, i, j)) return false;

    return true;
}

bool b_j_exists(const struct bap_inputs* const inputs, const index_t j) {
    FOR(i) if (!b_i_j_exists(inputs, i, j)) return false;

    return true;
}

bool allocation_exists(const struct bap_inputs* const inputs,
                       const index_t capturable) {
    return inputs->capturables[capturable] > 0;
}

bool surplus_j_exists(const struct bap_inputs* const inputs, const index_t j) {
    return a_j_exists(inputs, j) || b_j_exists(inputs, j);
}

bool problem_exists(const struct bap_inputs* const inputs) {
    FOR(i) if (!b_i_exists(inputs, i)) return false;

    return true;
}

index_t count_distribution_rows(const struct bap_inputs* const inputs) {
    index_t count = 0;
    FOR(i)
    if (a_i_exists(inputs, i))
        ++count;
    return count;
}

index_t count_allocation_rows(const struct bap_inputs* const inputs) {
    index_t count = 0;
    for (tile_t capturable = 0; capturable < CAPTURABLE_CAPACITY; ++capturable)
        if (allocation_exists(inputs, capturable))
            ++count;
    return count;
}

index_t count_budget_row(const struct bap_inputs* const inputs) { return 1; }

index_t count_surplus_rows(const struct bap_inputs* const inputs) {
    index_t count = 0;
    FOR(j)
    if (surplus_j_exists(inputs, j))
        ++count;
    return count;
}

index_t count_rows(const struct bap_inputs* const inputs) {
    return count_distribution_rows(inputs) + count_allocation_rows(inputs) +
           count_budget_row(inputs) + count_surplus_rows(inputs);
}

#define SYMBOLIC_NAME_LENGTH 16

void set_next_row(struct bap_temps* const temps, const char variable_name,
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
                           struct bap_temps* const temps) {
    temps->distribution_row_start_index = temps->curr_index;
    FOR(i)
    if (a_i_exists(inputs, i))
        set_next_row(temps, 'd', i, GLP_FX, inputs->friendly_distribution[i],
                     inputs->friendly_distribution[i]);
    temps->distribution_row_end_index = temps->curr_index;
}

void set_allocation_rows(const struct bap_inputs* const inputs,
                         struct bap_temps* const temps) {
    temps->allocation_row_start_index = temps->curr_index;
    for (tile_t capturable = 0; capturable < CAPTURABLE_CAPACITY; ++capturable)
        if (allocation_exists(inputs, capturable))
            set_next_row(temps, 'a', capturable, GLP_UP, 0.0,
                         (double)inputs->capturables[capturable]);
}

void set_budget_row(const struct bap_inputs* const inputs,
                    struct bap_temps* const temps) {
    temps->budget_row_index = temps->curr_index;
    set_next_row(temps, 'b', 0, GLP_UP, 0.0, inputs->budget);
}

void set_surplus_rows(const struct bap_inputs* const inputs,
                      struct bap_temps* const temps) {
    temps->surplus_row_start_index = temps->curr_index;
    FOR(j)
    if (surplus_j_exists(inputs, j))
        set_next_row(temps, 's', j, GLP_LO, 0.0, 0.0);
    temps->surplus_row_end_index = temps->curr_index;
}

void set_rows(const struct bap_inputs* const inputs,
              struct bap_temps* const temps) {
    temps->curr_index = 0;
    set_distribution_rows(inputs, temps);
    set_allocation_rows(inputs, temps);
    set_budget_row(inputs, temps);
    set_surplus_rows(inputs, temps);
}

void create_rows(const struct bap_inputs* const inputs,
                 struct bap_temps* const temps) {
    glp_add_cols(temps->problem, count_rows(inputs));
    set_rows(inputs, temps);
}

index_t count_a_columns(const struct bap_inputs* const inputs) {
    index_t count = 0;
    FOR(i)
    FOR(j)
    if (a_i_j_exists(inputs, i, j))
        ++count;
    return count;
}

index_t count_b_columns(const struct bap_inputs* const inputs) {
    index_t count = 0;
    FOR(i)
    FOR(j)
    if (b_i_j_exists(inputs, i, j))
        ++count;
    return count;
}

index_t count_objective_column(const struct bap_inputs* const inputs) {
    return 1;
}

index_t count_columns(const struct bap_inputs* const inputs) {
    return count_a_columns(inputs) + count_b_columns(inputs) +
           count_objective_column(inputs);
}

void set_next_matrix_column(struct bap_temps* const temps,
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
                   struct bap_temps* const temps) {
    temps->a_column_start_index = temps->curr_index;
    FOR(i)
    FOR(j)
    if (a_i_j_exists(inputs, i, j))
        set_next_matrix_column(temps, 'A', i, j, GLP_DB,
                               (double)inputs->friendly_distribution[i],
                               GLP_CV);
    temps->a_column_end_index = temps->curr_index;
}

void set_b_columns(const struct bap_inputs* const inputs,
                   struct bap_temps* const temps) {
    temps->b_column_start_index = temps->curr_index;
    FOR(i)
    FOR(j)
    if (b_i_j_exists(inputs, i, j))
        set_next_matrix_column(temps, 'B', i, j, GLP_LO, 0.0, GLP_IV);
}

void set_z_column(struct bap_temps* const temps) {
    temps->z_column_index = temps->curr_index;
    glp_set_col_name(temps->problem, temps->curr_index, "z");
    glp_set_obj_coef(temps->problem, temps->curr_index, 1.0);
    glp_set_col_bnds(temps->problem, temps->curr_index, GLP_FR, 0.0, 0.0);
    ++temps->curr_index;
}

void set_columns(const struct bap_inputs* const inputs,
                 struct bap_temps* const temps) {
    temps->curr_index = 0;
    set_a_columns(inputs, temps);
    set_b_columns(inputs, temps);
    set_z_column(temps);
}

void create_columns(const struct bap_inputs* const inputs,
                    struct bap_temps* const temps) {
    glp_add_cols(temps->problem, count_columns(inputs));
    set_columns(inputs, temps);
}

void sparse_matrix_set(struct bap_temps* const temps, const int row,
                       const int column, const double value) {
    assert(temps->curr_index < SPARSE_MATRIX_LENGTH);
    ++temps->curr_index;
    temps->matrix_rows[temps->curr_index] = row + 1;
    temps->matrix_columns[temps->curr_index] = column + 1;
    temps->matrix_values[temps->curr_index] = value;
}

void set_distribution_submatrix(const struct bap_inputs* const inputs,
                                struct bap_temps* const temps) {
    for (index_t row = temps->distribution_row_start_index;
         row < temps->distribution_row_end_index; ++row)
        for (index_t column = temps->a_column_start_index;
             column < temps->a_column_end_index; ++column) {
            sparse_matrix_set(temps, row, column, 1.0);
        }
}

void set_allocation_submatrix(const struct bap_inputs* const inputs,
                              struct bap_temps* const temps) {
    for (tile_t capturable = 0; capturable < CAPTURABLE_CAPACITY; ++capturable)
        if (allocation_exists(inputs, capturable)) {
            index_t row = temps->allocation_row_start_index;
            FOR(i)
            if (buildable_models[capturable] <= i &&
                i < buildable_models[capturable + 1] && b_i_exists(inputs, i)) {
                for (index_t column = temps->b_column_start_index;
                     column < temps->a_column_end_index; ++column)
                    sparse_matrix_set(temps, row, column, 1.0);
                ++row;
            }
        }
}

void set_budget_submatrix(const struct bap_inputs* const inputs,
                          struct bap_temps* const temps) {
    index_t column = temps->b_column_start_index;
    FOR(i)
    FOR(j)
    if (b_i_j_exists(inputs, i, j)) {
        sparse_matrix_set(temps, temps->budget_row_index, column,
                          models_cost[i]);
        ++column;
    }
}

double calc_surplus_submatrix_value(const struct bap_inputs* const inputs,
                                    const index_t i, const index_t j) {
    return (double)units_damage[i][j] * (models_min_range[i] ? 0.5 : 1.0) /
           (double)inputs->enemy_distribution[j];
}

void set_surplus_submatrix(const struct bap_inputs* const inputs,
                           struct bap_temps* const temps) {
    index_t a_column = temps->a_column_start_index;
    index_t b_column = temps->b_column_start_index;
    FOR(i) {
        index_t row = temps->surplus_row_start_index;
        FOR(j) {
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
                struct bap_temps* const temps) {
    set_distribution_submatrix(inputs, temps);
    set_allocation_submatrix(inputs, temps);
    set_budget_submatrix(inputs, temps);
    set_surplus_submatrix(inputs, temps);
}