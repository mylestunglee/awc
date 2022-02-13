#ifndef optimise_h
#define optimise_h

#include "typedefs.h"
#include "units.h"

void optimise_build_allocations(const health_wide_t[model_capacity],
                                const health_wide_t[model_capacity],
                                const grid_wide_t[capturable_capacity],
                                const gold_t, grid_wide_t[model_capacity],
                                void* const);

#endif
