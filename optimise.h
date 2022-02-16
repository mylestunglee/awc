#ifndef OPTIMISE_H
#define OPTIMISE_H

#include "definitions.h"
#include "game.h"
#include "units.h"

void optimise_build_allocations(const health_wide_t[MODEL_CAPACITY],
                                const health_wide_t[MODEL_CAPACITY],
                                const grid_wide_t[CAPTURABLE_CAPACITY],
                                const gold_t, grid_wide_t[MODEL_CAPACITY],
                                void* const);

#endif
