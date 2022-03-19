#ifndef BAP_SOLVER_FIXTURE_HPP
#define BAP_SOLVER_FIXTURE_HPP

#include "../bap.h"
#include "../list.h"
#include <gtest/gtest.h>
#include <memory>

class bap_solver_fixture : public ::testing::Test {
protected:
    bap_solver_fixture();
    void solve();

    struct bap_inputs inputs;
    grid_wide_t outputs[MODEL_CAPACITY];

private:
    std::unique_ptr<struct list> workspace;
};

#endif