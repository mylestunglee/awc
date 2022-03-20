#ifndef BAP_BASE_SOLVER_FIXTURE_HPP
#define BAP_BASE_SOLVER_FIXTURE_HPP

#include "../bap.h"
#include "../list.h"
#include <gtest/gtest.h>
#include <memory>

struct bap_base_solver_fixture : public ::testing::Test {
    struct bap_inputs inputs = {0};
    grid_wide_t outputs[MODEL_CAPACITY] = {0};
};

#endif