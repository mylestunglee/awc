#include "bap_default_solver.h"
#include "unit_constants.h"
#include <assert.h>

#define FIRST_BUILDABLE_BUILDING 1
#define DEFAULT_UNIT 0

void bap_default_solve(const struct bap_inputs* const inputs,
                       grid_wide_t outputs[MODEL_CAPACITY]) {
    assert(FIRST_BUILDABLE_BUILDING < BUILDING_CAPACITY);
    assert(building_buildable_models[FIRST_BUILDABLE_BUILDING + 1] >
           DEFAULT_UNIT);
    const grid_wide_t budget_allocatable =
        inputs->budget / model_costs[DEFAULT_UNIT];
    const grid_wide_t building_allocatable =
        inputs->buildings[FIRST_BUILDABLE_BUILDING];
    outputs[DEFAULT_UNIT] = budget_allocatable < building_allocatable
                                ? budget_allocatable
                                : building_allocatable;
}
