#define EXPOSE_BAP_GLPK_SOLVER_INTERNALS
#include "../unit_constants.h"
#include "bap_glpk_solver_internal_fixture.hpp"
#include "test_constants.hpp"

TEST_F(bap_glpk_solver_internal_fixture, set_next_row) {
    glp_add_rows(problem, 1);

    set_next_row(temps, 'a', 2, GLP_DB, 3.0, 5.0);

    ASSERT_EQ(strcmp(glp_get_row_name(problem, 1), "a_2"), 0);
    ASSERT_EQ(glp_get_row_type(problem, 1), GLP_DB);
    ASSERT_EQ(glp_get_row_lb(problem, 1), 3.0);
    ASSERT_EQ(glp_get_row_ub(problem, 1), 5.0);
    ASSERT_EQ(temps->curr_index, 2);
}

TEST_F(bap_glpk_solver_internal_fixture, set_distribution_rows) {
    inputs->friendly_distribution[MODEL_INFANTRY] = HEALTH_MAX;
    inputs->enemy_distribution[MODEL_INFANTRY] = 1;
    glp_add_rows(problem, 1);

    set_distribution_rows(inputs, temps);

    ASSERT_EQ(temps->distribution_row_start_index, 1);
    ASSERT_EQ(strcmp(glp_get_row_name(problem, 1), "d_0"), 0);
    ASSERT_EQ(glp_get_row_type(problem, 1), GLP_FX);
    ASSERT_EQ(glp_get_row_lb(problem, 1), 1.0);
    ASSERT_EQ(glp_get_row_ub(problem, 1), 1.0);
    ASSERT_EQ(temps->curr_index, 2);
    ASSERT_EQ(temps->distribution_row_end_index, 2);
}

TEST_F(bap_glpk_solver_internal_fixture, set_allocation_rows) {
    inputs->capturables[CAPTURABLE_FACTORY] = 1;
    inputs->enemy_distribution[MODEL_INFANTRY] = 1;
    glp_add_rows(problem, 1);

    set_allocation_rows(inputs, temps);

    ASSERT_EQ(temps->allocation_row_index, 1);
    ASSERT_EQ(strcmp(glp_get_row_name(problem, 1), "a_1"), 0);
    ASSERT_EQ(glp_get_row_type(problem, 1), GLP_UP);
    ASSERT_EQ(glp_get_row_ub(problem, 1), 1.0);
    ASSERT_EQ(temps->curr_index, 2);
}

TEST_F(bap_glpk_solver_internal_fixture, set_budget_row) {
    inputs->budget = 2000;
    glp_add_rows(problem, 1);

    set_budget_row(inputs, temps);

    ASSERT_EQ(temps->budget_row_index, 1);
    ASSERT_EQ(strcmp(glp_get_row_name(problem, 1), "b_0"), 0);
    ASSERT_EQ(glp_get_row_type(problem, 1), GLP_UP);
    ASSERT_EQ(glp_get_row_ub(problem, 1), 2000.0);
    ASSERT_EQ(temps->curr_index, 2);
}

TEST_F(bap_glpk_solver_internal_fixture, set_surplus_rows) {
    inputs->friendly_distribution[MODEL_INFANTRY] = 1;
    inputs->enemy_distribution[MODEL_INFANTRY] = 1;
    glp_add_rows(problem, 1);

    set_surplus_rows(inputs, temps);

    ASSERT_EQ(temps->surplus_row_start_index, 1);
    ASSERT_EQ(strcmp(glp_get_row_name(problem, 1), "s_0"), 0);
    ASSERT_EQ(glp_get_row_type(problem, 1), GLP_LO);
    ASSERT_EQ(glp_get_row_lb(problem, 1), 0.0);
    ASSERT_EQ(temps->curr_index, 2);
    ASSERT_EQ(temps->surplus_row_end_index, 2);
}

TEST_F(bap_glpk_solver_internal_fixture, set_rows) {
    inputs->friendly_distribution[MODEL_INFANTRY] = 1;
    inputs->enemy_distribution[MODEL_INFANTRY] = 1;
    temps->curr_index = 0;
    glp_add_rows(problem, 3);

    set_rows(inputs, temps);

    ASSERT_EQ(temps->curr_index, 4);
}

TEST_F(bap_glpk_solver_internal_fixture, create_rows) {
    inputs->friendly_distribution[MODEL_INFANTRY] = 1;
    inputs->enemy_distribution[MODEL_INFANTRY] = 1;
    temps->curr_index = 0;

    create_rows(inputs, temps);

    ASSERT_EQ(glp_get_num_rows(problem), 3);
    ASSERT_EQ(temps->curr_index, 4);
}

TEST_F(bap_glpk_solver_internal_fixture, set_next_matrix_column) {
    glp_add_cols(problem, 1);

    set_next_matrix_column(temps, 'A', 2, 3, GLP_DB, 5.0, GLP_CV);

    ASSERT_EQ(strcmp(glp_get_col_name(problem, 1), "A_2_3"), 0);
    ASSERT_EQ(glp_get_col_type(problem, 1), GLP_DB);
    ASSERT_EQ(glp_get_col_lb(problem, 1), 0.0);
    ASSERT_EQ(glp_get_col_ub(problem, 1), 5.0);
    ASSERT_EQ(glp_get_col_kind(problem, 1), GLP_CV);
    ASSERT_EQ(temps->curr_index, 2);
}

TEST_F(bap_glpk_solver_internal_fixture, set_a_columns) {
    inputs->friendly_distribution[MODEL_INFANTRY] = HEALTH_MAX;
    inputs->enemy_distribution[MODEL_INFANTRY] = 1;
    glp_add_cols(problem, 1);

    set_a_columns(inputs, temps);

    ASSERT_EQ(temps->a_column_start_index, 1);
    ASSERT_EQ(strcmp(glp_get_col_name(problem, 1), "A_0_0"), 0);
    ASSERT_EQ(glp_get_col_type(problem, 1), GLP_DB);
    ASSERT_EQ(glp_get_col_ub(problem, 1), 1.0);
    ASSERT_EQ(glp_get_col_kind(problem, 1), GLP_CV);
    ASSERT_EQ(temps->curr_index, 2);
    ASSERT_EQ(temps->a_column_end_index, 2);
}

TEST_F(bap_glpk_solver_internal_fixture, set_b_columns) {
    inputs->capturables[CAPTURABLE_FACTORY] = 1;
    inputs->enemy_distribution[MODEL_INFANTRY] = 1;
    glp_add_cols(problem, 8);

    set_b_columns(inputs, temps);

    ASSERT_EQ(temps->b_column_index, 1);
    ASSERT_EQ(strcmp(glp_get_col_name(problem, 1), "B_0_0"), 0);
    ASSERT_EQ(glp_get_col_type(problem, 1), GLP_LO);
    ASSERT_EQ(glp_get_col_kind(problem, 1), GLP_IV);
    ASSERT_EQ(temps->curr_index, 9);
}

TEST_F(bap_glpk_solver_internal_fixture, set_z_column) {
    glp_add_cols(problem, 1);

    set_z_column(temps);

    ASSERT_EQ(temps->z_column_index, 1);
    ASSERT_EQ(strcmp(glp_get_col_name(problem, 1), "z"), 0);
    ASSERT_EQ(glp_get_col_type(problem, 1), GLP_FR);
    ASSERT_EQ(glp_get_col_kind(problem, 1), GLP_CV);
    ASSERT_EQ(temps->curr_index, 2);
}

TEST_F(bap_glpk_solver_internal_fixture, set_columns) {
    inputs->friendly_distribution[MODEL_INFANTRY] = 1;
    inputs->enemy_distribution[MODEL_INFANTRY] = 1;
    temps->curr_index = 0;
    glp_add_cols(problem, 2);

    set_columns(inputs, temps);

    ASSERT_EQ(temps->curr_index, 3);
}

TEST_F(bap_glpk_solver_internal_fixture, create_columns) {
    inputs->friendly_distribution[MODEL_INFANTRY] = 1;
    inputs->enemy_distribution[MODEL_INFANTRY] = 1;
    temps->curr_index = 0;

    create_columns(inputs, temps);

    ASSERT_EQ(glp_get_num_cols(problem), 2);
    ASSERT_EQ(temps->curr_index, 3);
}

TEST_F(bap_glpk_solver_internal_fixture, sparse_matrix_set) {
    sparse_matrix_set(temps, 3, 5, 7.0);

    ASSERT_EQ(temps->matrix_rows[1], 3);
    ASSERT_EQ(temps->matrix_columns[1], 5);
    ASSERT_EQ(temps->matrix_values[1], 7.0);
    ASSERT_EQ(temps->curr_index, 2);
}

TEST_F(bap_glpk_solver_internal_fixture, set_distribution_submatrix) {
    temps->distribution_row_start_index = 2;
    temps->distribution_row_end_index = 3;
    temps->a_column_start_index = 5;
    temps->a_column_end_index = 6;

    set_distribution_submatrix(temps);

    ASSERT_EQ(temps->matrix_values[1], 1.0);
    ASSERT_EQ(temps->curr_index, 2);
}

TEST_F(bap_glpk_solver_internal_fixture, set_allocation_submatrix) {
    inputs->capturables[CAPTURABLE_FACTORY] = 1;
    inputs->enemy_distribution[MODEL_INFANTRY] = 1;

    set_allocation_submatrix(inputs, temps);

    ASSERT_EQ(temps->matrix_values[1], 1.0);
    ASSERT_EQ(temps->curr_index, 9);
}

TEST_F(bap_glpk_solver_internal_fixture, set_budget_submatrix) {
    inputs->capturables[CAPTURABLE_FACTORY] = 1;
    inputs->enemy_distribution[MODEL_INFANTRY] = 1;

    set_budget_submatrix(inputs, temps);

    ASSERT_EQ(temps->matrix_values[1], models_cost[MODEL_INFANTRY]);
    ASSERT_EQ(temps->curr_index, 9);
}

TEST_F(bap_glpk_solver_internal_fixture, calc_surplus_submatrix_value) {
    inputs->enemy_distribution[MODEL_INFANTRY] = 5;
    ASSERT_EQ(
        calc_surplus_submatrix_value(inputs, MODEL_ARTILLERY, MODEL_INFANTRY),
        90.0 * 0.5 / 5.0);
}

TEST_F(bap_glpk_solver_internal_fixture, set_surplus_submatrix) {
    inputs->capturables[CAPTURABLE_FACTORY] = 1;
    inputs->friendly_distribution[MODEL_INFANTRY] = 1;
    inputs->enemy_distribution[MODEL_INFANTRY] = 1;
    temps->surplus_row_start_index = 2;
    temps->surplus_row_end_index = 3;

    set_surplus_submatrix(inputs, temps);

    ASSERT_EQ(temps->matrix_values[1], 55.0);
    ASSERT_EQ(temps->matrix_values[2], 55.0);
    ASSERT_EQ(temps->matrix_values[10], -1.0);
    ASSERT_EQ(temps->curr_index, 11);
}

TEST_F(bap_glpk_solver_internal_fixture, set_matrix) {
    inputs->capturables[CAPTURABLE_FACTORY] = 1;
    inputs->friendly_distribution[MODEL_INFANTRY] = 1;
    inputs->enemy_distribution[MODEL_INFANTRY] = 1;

    create_rows(inputs, temps);
    create_columns(inputs, temps);

    set_matrix(inputs, temps);

    ASSERT_EQ(temps->curr_index, 28);
}

TEST_F(bap_glpk_solver_internal_fixture, bap_glpk_temps_preinitialise) {
    temps->problem = nullptr;
    temps->matrix_rows[0] = 2;
    temps->matrix_columns[0] = 3;
    temps->matrix_values[0] = 5.0;

    bap_glpk_temps_preinitialise(temps);

    ASSERT_NE(temps->problem, nullptr);
    ASSERT_EQ(temps->matrix_rows[0], 0);
    ASSERT_EQ(temps->matrix_columns[0], 0);
    ASSERT_EQ(temps->matrix_values[0], 0.0);
}

TEST_F(bap_glpk_solver_internal_fixture, bap_glpk_temps_initialise) {
    inputs->capturables[CAPTURABLE_FACTORY] = 1;
    inputs->friendly_distribution[MODEL_INFANTRY] = 1;
    inputs->enemy_distribution[MODEL_INFANTRY] = 1;

    bap_glpk_temps_initialise(inputs, temps);

    ASSERT_EQ(temps->curr_index, 28);
}
