#include "../src/bap_default_solver.h"
#include "../src/constants.h"
#include "bap_base_solver_fixture.hpp"
#include "test_constants.hpp"
#include <gtest/gtest.h>

struct bap_default_solver_fixture : public bap_base_solver_fixture {
    void solve() { bap_default_solve(&inputs, outputs); }
};

TEST_F(bap_default_solver_fixture,
       bap_default_solve_when_bounded_by_buildings) {
    inputs.budget = 3 * MONEY_SCALE;
    inputs.buildings[BUILDING_FACTORY] = 2;

    solve();

    ASSERT_EQ(outputs[MODEL_INFANTRY], 2);
}

TEST_F(bap_default_solver_fixture, bap_default_solve_when_bounded_by_budget) {
    inputs.budget = 2 * MONEY_SCALE;
    inputs.buildings[BUILDING_FACTORY] = 3;

    solve();

    ASSERT_EQ(outputs[MODEL_INFANTRY], 2);
}
