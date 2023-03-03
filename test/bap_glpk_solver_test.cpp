#define EXPOSE_BAP_GLPK_SOLVER_INTERNALS
#include "../src/bap_glpk_solver.h"
#include "../src/constants.h"
#include "../src/unit_constants.h"
#include "bap_base_solver_fixture.hpp"
#include "test_constants.hpp"

class bap_glpk_solver_fixture : public bap_base_solver_fixture {
public:
    int solve() { return bap_glpk_solve(&inputs, outputs, workspace.get()); }

private:
    std::unique_ptr<struct list> workspace = std::make_unique<struct list>();
};

TEST_F(bap_glpk_solver_fixture, constant_functions_when_null_case) {
    ASSERT_FALSE(a_i_j_exists(&inputs, MODEL_INFANTRY, MODEL_INFANTRY));
    ASSERT_FALSE(a_i_exists(&inputs, MODEL_INFANTRY));
    ASSERT_FALSE(a_j_exists(&inputs, MODEL_INFANTRY));
    ASSERT_FALSE(surplus_j_exists(&inputs, MODEL_INFANTRY));
    ASSERT_FALSE(bap_glpk_solvable(&inputs));

    ASSERT_EQ(count_distribution_rows(&inputs), 0);
    ASSERT_EQ(count_allocation_rows(&inputs), 0);
    ASSERT_EQ(count_budget_row(), 1);
    ASSERT_EQ(count_surplus_rows(&inputs), 0);
    ASSERT_EQ(count_rows(&inputs), 1);

    ASSERT_EQ(count_a_columns(&inputs), 0);
    ASSERT_EQ(count_b_columns(&inputs), 0);
    ASSERT_EQ(count_objective_column(), 1);
    ASSERT_EQ(count_columns(&inputs), 1);
}

TEST_F(bap_glpk_solver_fixture, constant_functions_when_single_distribution) {
    inputs.friendly_distribution[MODEL_INFANTRY] = 1;
    inputs.enemy_distribution[MODEL_INFANTRY] = 1;

    ASSERT_TRUE(a_i_j_exists(&inputs, MODEL_INFANTRY, MODEL_INFANTRY));
    ASSERT_TRUE(a_i_exists(&inputs, MODEL_INFANTRY));
    ASSERT_TRUE(a_j_exists(&inputs, MODEL_INFANTRY));
    ASSERT_TRUE(surplus_j_exists(&inputs, MODEL_INFANTRY));
    ASSERT_FALSE(bap_glpk_solvable(&inputs));

    ASSERT_EQ(count_distribution_rows(&inputs), 1);
    ASSERT_EQ(count_allocation_rows(&inputs), 0);
    ASSERT_EQ(count_budget_row(), 1);
    ASSERT_EQ(count_surplus_rows(&inputs), 1);
    ASSERT_EQ(count_rows(&inputs), 3);

    ASSERT_EQ(count_a_columns(&inputs), 1);
    ASSERT_EQ(count_b_columns(&inputs), 0);
    ASSERT_EQ(count_objective_column(), 1);
    ASSERT_EQ(count_columns(&inputs), 2);
}

TEST_F(bap_glpk_solver_fixture, constant_functions_when_single_allocation) {
    inputs.buildings[BUILDING_FACTORY] = 1;
    inputs.enemy_distribution[MODEL_INFANTRY] = 1;

    ASSERT_TRUE(b_i_j_exists(&inputs, MODEL_INFANTRY, MODEL_INFANTRY));
    ASSERT_TRUE(b_i_exists(&inputs, MODEL_INFANTRY));
    ASSERT_TRUE(b_j_exists(&inputs, MODEL_INFANTRY));
    ASSERT_TRUE(surplus_j_exists(&inputs, MODEL_INFANTRY));
    ASSERT_TRUE(bap_glpk_solvable(&inputs));

    ASSERT_EQ(count_distribution_rows(&inputs), 0);
    ASSERT_EQ(count_allocation_rows(&inputs), 1);
    ASSERT_EQ(count_budget_row(), 1);
    ASSERT_EQ(count_surplus_rows(&inputs), 1);
    ASSERT_EQ(count_rows(&inputs), 3);

    ASSERT_EQ(count_a_columns(&inputs), 0);
    ASSERT_EQ(count_b_columns(&inputs), 8);
    ASSERT_EQ(count_objective_column(), 1);
    ASSERT_EQ(count_columns(&inputs), 9);
}

class bap_glpk_solver_internal_fixture : public ::testing::Test {
protected:
    bap_glpk_solver_internal_fixture()
        : managed_inputs(std::make_unique<struct bap_inputs>()),
          managed_temps(std::make_unique<struct bap_glpk_temps>()),
          inputs(managed_inputs.get()), temps(managed_temps.get()),
          problem(managed_temps->problem) {
        bap_glpk_temps_preinitialise(temps);
        temps->curr_index = 1;
    }

private:
    std::unique_ptr<struct bap_inputs> managed_inputs;
    std::unique_ptr<struct bap_glpk_temps> managed_temps;

protected:
    struct bap_inputs* inputs;
    struct bap_glpk_temps* temps;
    glp_prob*& problem;
};

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

    ASSERT_EQ(temps->distribution_row_index, 1);
    ASSERT_EQ(strcmp(glp_get_row_name(problem, 1), "d_0"), 0);
    ASSERT_EQ(glp_get_row_type(problem, 1), GLP_FX);
    ASSERT_EQ(glp_get_row_lb(problem, 1), 1.0);
    ASSERT_EQ(glp_get_row_ub(problem, 1), 1.0);
    ASSERT_EQ(temps->curr_index, 2);
}

TEST_F(bap_glpk_solver_internal_fixture, set_allocation_rows) {
    inputs->buildings[BUILDING_FACTORY] = 1;
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

    ASSERT_EQ(temps->a_column_index, 1);
    ASSERT_EQ(strcmp(glp_get_col_name(problem, 1), "A_0_0"), 0);
    ASSERT_EQ(glp_get_col_type(problem, 1), GLP_DB);
    ASSERT_EQ(glp_get_col_ub(problem, 1), 1.0);
    ASSERT_EQ(glp_get_col_kind(problem, 1), GLP_CV);
    ASSERT_EQ(temps->curr_index, 2);
}

TEST_F(bap_glpk_solver_internal_fixture, set_b_columns) {
    inputs->buildings[BUILDING_FACTORY] = 1;
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
    inputs->friendly_distribution[MODEL_INFANTRY] = 1;
    inputs->enemy_distribution[MODEL_INFANTRY] = 1;
    temps->distribution_row_index = 2;
    temps->a_column_index = 3;

    set_distribution_submatrix(inputs, temps);

    ASSERT_EQ(temps->matrix_values[1], 1.0);
    ASSERT_EQ(temps->curr_index, 2);
}

TEST_F(bap_glpk_solver_internal_fixture, set_allocation_submatrix) {
    inputs->buildings[BUILDING_FACTORY] = 1;
    inputs->enemy_distribution[MODEL_INFANTRY] = 1;

    set_allocation_submatrix(inputs, temps);

    ASSERT_EQ(temps->matrix_values[1], 1.0);
    ASSERT_EQ(temps->curr_index, 9);
}

TEST_F(bap_glpk_solver_internal_fixture, set_budget_submatrix) {
    inputs->buildings[BUILDING_FACTORY] = 1;
    inputs->enemy_distribution[MODEL_INFANTRY] = 1;

    set_budget_submatrix(inputs, temps);

    ASSERT_EQ(temps->matrix_values[1], model_costs[MODEL_INFANTRY]);
    ASSERT_EQ(temps->curr_index, 9);
}

TEST_F(bap_glpk_solver_internal_fixture, calc_surplus_submatrix_value) {
    inputs->enemy_distribution[MODEL_INFANTRY] = 5;
    ASSERT_EQ(
        calc_surplus_submatrix_value(inputs, MODEL_ARTILLERY, MODEL_INFANTRY),
        90.0 * 0.5 / 5.0);
}

TEST_F(bap_glpk_solver_internal_fixture, set_surplus_submatrix) {
    inputs->buildings[BUILDING_FACTORY] = 1;
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
    inputs->buildings[BUILDING_FACTORY] = 1;
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
    inputs->buildings[BUILDING_FACTORY] = 1;
    inputs->friendly_distribution[MODEL_INFANTRY] = 1;
    inputs->enemy_distribution[MODEL_INFANTRY] = 1;

    bap_glpk_temps_initialise(inputs, temps);

    ASSERT_EQ(temps->curr_index, 28);
}

TEST_F(bap_glpk_solver_fixture, bap_glpk_solve_null_problem) {
    ASSERT_NE(solve(), 0);
}

TEST_F(bap_glpk_solver_fixture, bap_glpk_solve_single_infantry) {
    inputs.enemy_distribution[MODEL_INFANTRY] = 1;
    inputs.buildings[BUILDING_FACTORY] = 1;
    inputs.budget = model_costs[MODEL_INFANTRY];
    ASSERT_EQ(solve(), 0);
    ASSERT_EQ(outputs[MODEL_INFANTRY], 1);
}

TEST_F(bap_glpk_solver_fixture, bap_glpk_solve_constrained_by_buildings) {
    inputs.enemy_distribution[MODEL_INFANTRY] = 1;
    inputs.buildings[BUILDING_FACTORY] = 2;
    inputs.budget = 3 * model_costs[MODEL_INFANTRY];
    ASSERT_EQ(solve(), 0);
    ASSERT_EQ(outputs[MODEL_INFANTRY], 2);
}

TEST_F(bap_glpk_solver_fixture, bap_glpk_solve_constrained_by_budget) {
    inputs.enemy_distribution[MODEL_INFANTRY] = 1;
    inputs.buildings[BUILDING_FACTORY] = 3;
    inputs.budget = 2 * model_costs[MODEL_INFANTRY];
    ASSERT_EQ(solve(), 0);
    ASSERT_EQ(outputs[MODEL_INFANTRY], 2);
}

TEST_F(bap_glpk_solver_fixture, bap_glpk_solve_selects_most_effective_unit) {
    inputs.enemy_distribution[MODEL_HELICOPTER] = 1;
    inputs.buildings[BUILDING_AIRPORT] = 1;
    inputs.budget = 100 * MONEY_SCALE;
    ASSERT_EQ(solve(), 0);
    ASSERT_EQ(outputs[MODEL_FIGHTER], 1);
}

TEST_F(bap_glpk_solver_fixture, bap_glpk_solve_distributes_ratio_across_units) {
    inputs.friendly_distribution[MODEL_SUBMARINE] = 2 * HEALTH_MAX;
    inputs.enemy_distribution[MODEL_HELICOPTER] = 1;
    inputs.enemy_distribution[MODEL_SUBMARINE] = 1;
    inputs.buildings[BUILDING_AIRPORT] = 1;
    inputs.budget = 100 * MONEY_SCALE;
    ASSERT_EQ(solve(), 0);
    ASSERT_EQ(outputs[MODEL_FIGHTER], 1);
}
