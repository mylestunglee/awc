#define EXPOSE_BAP_GLPK_SOLVER_INTERNALS
#include "../bap_glpk_solver.h"
#include "../constants.h"
#include "../unit_constants.h"
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
    inputs.capturables[CAPTURABLE_FACTORY] = 1;
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

TEST_F(bap_glpk_solver_fixture, bap_glpk_solve_null_problem) {
    ASSERT_NE(solve(), 0);
}

TEST_F(bap_glpk_solver_fixture, bap_glpk_solve_single_infantry) {
    inputs.enemy_distribution[MODEL_INFANTRY] = 1;
    inputs.capturables[CAPTURABLE_FACTORY] = 1;
    inputs.budget = model_cost[MODEL_INFANTRY];
    ASSERT_EQ(solve(), 0);
    ASSERT_EQ(outputs[MODEL_INFANTRY], 1);
}

TEST_F(bap_glpk_solver_fixture, bap_glpk_solve_constrained_by_capturables) {
    inputs.enemy_distribution[MODEL_INFANTRY] = 1;
    inputs.capturables[CAPTURABLE_FACTORY] = 2;
    inputs.budget = 3 * model_cost[MODEL_INFANTRY];
    ASSERT_EQ(solve(), 0);
    ASSERT_EQ(outputs[MODEL_INFANTRY], 2);
}

TEST_F(bap_glpk_solver_fixture, bap_glpk_solve_constrained_by_budget) {
    inputs.enemy_distribution[MODEL_INFANTRY] = 1;
    inputs.capturables[CAPTURABLE_FACTORY] = 3;
    inputs.budget = 2 * model_cost[MODEL_INFANTRY];
    ASSERT_EQ(solve(), 0);
    ASSERT_EQ(outputs[MODEL_INFANTRY], 2);
}

TEST_F(bap_glpk_solver_fixture, bap_glpk_solve_selects_most_effective_unit) {
    inputs.enemy_distribution[MODEL_HELICOPTER] = 1;
    inputs.capturables[CAPTURABLE_AIRPORT] = 1;
    inputs.budget = 100 * GOLD_SCALE;
    ASSERT_EQ(solve(), 0);
    ASSERT_EQ(outputs[MODEL_FIGHTER], 1);
}

TEST_F(bap_glpk_solver_fixture, bap_glpk_solve_distributes_ratio_across_units) {
    inputs.friendly_distribution[MODEL_SUBMARINE] = 1;
    inputs.enemy_distribution[MODEL_HELICOPTER] = 1;
    inputs.enemy_distribution[MODEL_SUBMARINE] = 1;
    inputs.capturables[CAPTURABLE_AIRPORT] = 1;
    inputs.budget = 100 * GOLD_SCALE;
    ASSERT_EQ(solve(), 0);
    ASSERT_EQ(outputs[MODEL_FIGHTER], 1);
}
