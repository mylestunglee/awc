#define EXPOSE_BAP_GLPK_SOLVER_INTERNALS
#include "bap_glpk_solver_fixture.hpp"

TEST_F(bap_glpk_solver_fixture, bap_glpk_solve_null_problem) {
    ASSERT_EQ(solve(), 0);
}
