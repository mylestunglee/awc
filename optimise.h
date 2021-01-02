#ifndef optimise_h
#define optimise_h

#include "definitions.h"

void optimise_build_allocations(
	const health_wide_t[model_capacity],
	const health_wide_t[model_capacity],
	const tile_wide_t[model_capacity],
	const gold_t,
	double[model_capacity],
	void* const);

#endif
