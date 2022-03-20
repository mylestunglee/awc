#include "../bap_solver.h"
#include "../unit_constants.h"
#include "bap_base_solver_fixture.hpp"
#include "test_constants.hpp"
#include <gtest/gtest.h>

class bap_solver_fixture : public bap_base_solver_fixture {
public:
    void solve() { bap_solve(&inputs, outputs, workspace.get()); }

private:
    std::unique_ptr<struct list> workspace = std::make_unique<struct list>();
};

TEST_F(bap_solver_fixture, bap_solve_when_null_input) {
    solve();

    for (auto model = 0; model < MODEL_CAPACITY; ++model)
        ASSERT_EQ(outputs[model], 0);
}

TEST_F(bap_solver_fixture, bap_solve_when_allocatable) {
    inputs.enemy_distribution[MODEL_INFANTRY] = 1;
    inputs.capturables[CAPTURABLE_FACTORY] = 1;
    inputs.budget = models_cost[MODEL_INFANTRY];

    solve();

    ASSERT_EQ(outputs[MODEL_INFANTRY], 1);
}