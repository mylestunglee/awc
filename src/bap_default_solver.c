#include "bap_default_solver.h"
#include "unit_constants.h"
#include <assert.h>

#define FIRST_BUILDABLE_CAPTURABLE 1
#define DEFAULT_UNIT 0

void bap_default_solve(const struct bap_inputs* const inputs,
                       grid_wide_t outputs[MODEL_CAPACITY]) {
    assert(FIRST_BUILDABLE_CAPTURABLE < CAPTURABLE_CAPACITY);
    assert(capturable_buildable_models[FIRST_BUILDABLE_CAPTURABLE + 1] >
           DEFAULT_UNIT);
    const grid_wide_t budget_allocatable =
        inputs->budget / model_cost[DEFAULT_UNIT];
    const grid_wide_t capturable_allocatable =
        inputs->capturables[FIRST_BUILDABLE_CAPTURABLE];
    outputs[DEFAULT_UNIT] = budget_allocatable < capturable_allocatable
                                ? budget_allocatable
                                : capturable_allocatable;
}
