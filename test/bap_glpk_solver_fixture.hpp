#ifndef BAP_GLPK_SOLVER_FIXTURE_HPP
#define BAP_GLPK_SOLVER_FIXTURE_HPP

#include "../bap_glpk_solver.h"
#include "../list.h"
#include <gtest/gtest.h>
#include <memory>

class bap_glpk_solver_fixture : public ::testing::Test {
protected:
    bap_glpk_solver_fixture();
    int solve();

    std::unique_ptr<struct bap_inputs> inputs;
    grid_wide_t outputs[MODEL_CAPACITY];

private:
    std::unique_ptr<struct list> workspace;
};

#endif