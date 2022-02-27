#define EXPOSE_BAP_GLPK_SOLVER_INTERNALS
#include "bap_glpk_solver_fixture.hpp"
#include "../constants.h"
#include "../unit_constants.h"
#include "test_constants.hpp"

TEST_F(bap_glpk_solver_fixture, bap_glpk_solve_simple_distribution) {
    inputs->friendly_distribution[MODEL_INFANTRY] = 1;
    inputs->enemy_distribution[MODEL_INFANTRY] = 1;
    ASSERT_EQ(solve(), 0);
}
