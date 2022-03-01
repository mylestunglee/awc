#define EXPOSE_BAP_GLPK_SOLVER_INTERNALS
#include "bap_glpk_solver_internal_fixture.hpp"
#include "test_constants.hpp"
#include <glpk.h>

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