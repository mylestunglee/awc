#ifndef optimise_h
#define optimise_h

#include "definitions.h"

void optimise_build_allocations(
	const health_wide_t[model_capacity],
	const health_wide_t[model_capacity],
	const tile_wide_t[model_capacity],
	const gold_t,
	tile_wide_t[model_capacity],
	void* const);

#define sparse_matrix_length (1 + model_capacity + 4 * model_capacity * model_capacity)

struct sparse_matrix {
	int is[sparse_matrix_length];
	int js[sparse_matrix_length];
	double values[sparse_matrix_length];
};

#endif
