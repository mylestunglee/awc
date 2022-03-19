#include "../bap_default_solver.h"
#include "../constants.h"
#include "bap_solver_fixture.hpp"
#include "test_constants.hpp"
#include <gtest/gtest.h>

constexpr auto FIRST_CAPTURABLE = 1;

struct bap_default_solver_fixture : public bap_solver_fixture {
    void solve() { bap_default_solve(&inputs, outputs); }
};

TEST_F(bap_default_solver_fixture,
       bap_default_solve_when_bounded_by_capturables) {
    inputs.budget = 3 * GOLD_SCALE;
    inputs.capturables[FIRST_CAPTURABLE] = 2;

    solve();

    ASSERT_EQ(outputs[MODEL_INFANTRY], 2);
}

TEST_F(bap_default_solver_fixture, bap_default_solve_when_bounded_by_budget) {
    inputs.budget = 2 * GOLD_SCALE;
    inputs.capturables[FIRST_CAPTURABLE] = 3;

    solve();

    ASSERT_EQ(outputs[MODEL_INFANTRY], 2);
}