#define EXPOSE_BAP_GLPK_SOLVER_INTERNALS
#include "../constants.h"
#include "../unit_constants.h"
#include "bap_glpk_solver_fixture.hpp"
#include "test_constants.hpp"

constexpr auto CAPTURABLE_FACTORY = 1;
constexpr auto CAPTURABLE_AIRPORT = 2;

TEST_F(bap_glpk_solver_fixture, bap_glpk_solve_null_problem) {
    ASSERT_NE(solve(), 0);
}

TEST_F(bap_glpk_solver_fixture, bap_glpk_solve_single_infantry) {
    inputs->enemy_distribution[MODEL_INFANTRY] = 1;
    inputs->capturables[CAPTURABLE_FACTORY] = 1;
    inputs->budget = models_cost[MODEL_INFANTRY];
    ASSERT_EQ(solve(), 0);
    ASSERT_EQ(outputs[MODEL_INFANTRY], 1);
}

TEST_F(bap_glpk_solver_fixture, bap_glpk_solve_constrained_by_capturables) {
    inputs->enemy_distribution[MODEL_INFANTRY] = 1;
    inputs->capturables[CAPTURABLE_FACTORY] = 2;
    inputs->budget = 3 * models_cost[MODEL_INFANTRY];
    ASSERT_EQ(solve(), 0);
    ASSERT_EQ(outputs[MODEL_INFANTRY], 2);
}

TEST_F(bap_glpk_solver_fixture, bap_glpk_solve_constrained_by_budget) {
    inputs->enemy_distribution[MODEL_INFANTRY] = 1;
    inputs->capturables[CAPTURABLE_FACTORY] = 3;
    inputs->budget = 2 * models_cost[MODEL_INFANTRY];
    ASSERT_EQ(solve(), 0);
    ASSERT_EQ(outputs[MODEL_INFANTRY], 2);
}

TEST_F(bap_glpk_solver_fixture, bap_glpk_solve_selects_most_effective_unit) {
    inputs->enemy_distribution[MODEL_HELICOPTER] = 1;
    inputs->capturables[CAPTURABLE_AIRPORT] = 1;
    inputs->budget = 100 * GOLD_SCALE;
    ASSERT_EQ(solve(), 0);
    ASSERT_EQ(outputs[MODEL_FIGHTER], 1);
}