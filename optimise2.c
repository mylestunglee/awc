#include "optimise2.h"
#include "unit_constants.h"
#include <stdio.h>

#define FOR(variable, condition)                                               \
    for (index_t variable = 0; variable < MODEL_CAPACITY; ++variable)          \
        if (condition)

bool a_cell_exists(const struct bap_inputs* const inputs, const index_t i,
                   const index_t j) {
    return inputs->friendly_distribution[i] > 0 &&
           inputs->enemy_distribution[j] > 0 && units_damage[j][i] > 0;
}

bool a_row_exists(const struct bap_inputs* const inputs, const index_t i) {
    FOR(j, !a_cell_exists(inputs, i, j)) return false;

    return true;
}

bool a_column_exists(const struct bap_inputs* const inputs, const index_t j) {
    FOR(i, !a_cell_exists(inputs, i, j)) return false;

    return true;
}

bool b_cell_exists(const struct bap_inputs* const inputs, const index_t i,
                   const index_t j) {
    FOR(i, !a_cell_exists(inputs, i, j)) return false;

    return true;
}

bool b_row_exists(const struct bap_inputs* const inputs, const index_t i) {
    FOR(j, !b_cell_exists(inputs, i, j)) return false;

    return true;
}

bool b_column_exists(const struct bap_inputs* const inputs, const index_t j) {
    FOR(i, !b_cell_exists(inputs, i, j)) return false;

    return true;
}

bool s_column_exists(const struct bap_inputs* const inputs, const index_t j) {
    return a_column_exists(inputs, j) || b_column_exists(inputs, j);
}

#define SYMBOLIC_NAME_LENGTH 16

void create_row(struct bap_temps* const temps, const char variable_name,
                const index_t variable_index, int bounds_type,
                double lower_bound, double upper_bound) {
    char name[SYMBOLIC_NAME_LENGTH];
    snprintf(name, SYMBOLIC_NAME_LENGTH, "%c_%hu", variable_name,
             variable_index);
    glp_set_row_name(temps->problem, temps->curr_index, name);
    glp_set_row_bnds(temps->problem, temps->curr_index, bounds_type,
                     lower_bound, upper_bound);
    ++temps->curr_index;
}

void create_a_rows(const struct bap_inputs* const inputs,
                   struct bap_temps* const temps) {
    temps->a_row_index = temps->curr_index;
    FOR(i, a_row_exists(inputs, i))
    create_row(temps, 'a', i, GLP_DB, 0.0, inputs->friendly_distribution[i]);
}

void create_a_columns(const struct bap_inputs* const inputs,
                      struct bap_temps* const temps) {
    temps->a_column_index = temps->curr_index;
}