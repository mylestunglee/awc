#ifndef BAP_GLPK_SOLVER_INTERNAL_FIXTURE_HPP
#define BAP_GLPK_SOLVER_INTERNAL_FIXTURE_HPP

#include "../bap_glpk_solver.h"
#include <gtest/gtest.h>
#include <memory>

class bap_glpk_solver_internal_fixture : public ::testing::Test {
protected:
    bap_glpk_solver_internal_fixture();

private:
    std::unique_ptr<struct bap_inputs> managed_inputs;
    std::unique_ptr<struct bap_glpk_temps> managed_temps;

protected:
    struct bap_inputs* inputs;
    struct bap_glpk_temps* temps;
    glp_prob*& problem;
};

#endif
