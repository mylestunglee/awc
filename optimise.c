#include "optimise.h"

#include <glpk.h>
#include <stdio.h>
#define symbolic_name_length 16

static void add_distribution_rows(glp_prob* const problem, int* const row_offset) {
	glp_add_rows(problem, model_capacity);

	for (model_t model = 0; model < model_capacity; ++model) {
		char name[symbolic_name_length];
		snprintf(name, symbolic_name_length, "u_"model_format, model + 1);
		glp_set_row_name(problem, *row_offset, name);
		glp_set_row_bnds(problem, *row_offset, GLP_DB, 0.0, 1.0);
		++*row_offset;
	}
}

static void add_allocation_rows(
	glp_prob* const problem,
	const tile_wide_t buildable_allocations[model_capacity],
	int* const row_offset) {

	glp_add_rows(problem, model_capacity);

	for (model_t model = 0; model < model_capacity; ++model) {
		char name[symbolic_name_length];
		snprintf(name, symbolic_name_length, "v_"model_format, model + 1);
		glp_set_row_name(problem, *row_offset, name);

		// Row lower and upper bounds cannot be equal with GLP_DB
		if (buildable_allocations[model])
			glp_set_row_bnds(problem, *row_offset, GLP_DB, 0.0, (double)buildable_allocations[model]);
		else
			glp_set_row_bnds(problem, *row_offset, GLP_FX, 0.0, 0.0);

		++*row_offset;
	}
}

static void add_cost_row(glp_prob* const problem, const gold_t budget, int* const row_offset) {
	glp_add_rows(problem, 1);
	glp_set_row_name(problem, *row_offset, "b");
	glp_set_row_bnds(problem, *row_offset, GLP_UP, 0.0, budget);
	++*row_offset;
}

static void add_surplus_rows(glp_prob* const problem, int* const row_offset) {
	glp_add_rows(problem, model_capacity);

	for (model_t model = 0; model < model_capacity; ++model) {
		char name[symbolic_name_length];
		snprintf(name, symbolic_name_length, "s_"model_format, model + 1);
		glp_set_row_name(problem, *row_offset, name);
		glp_set_row_bnds(problem, *row_offset, GLP_LO, 0.0, 0.0);
		++*row_offset;
	}
}

static void add_rows(
	glp_prob* const problem,
	const tile_wide_t buildable_allocations[model_capacity],
	const gold_t budget) {

	int row_offset = 1;
	add_distribution_rows(problem, &row_offset);
	add_allocation_rows(problem, buildable_allocations, &row_offset);
	add_cost_row(problem, budget, &row_offset);
	add_surplus_rows(problem, &row_offset);
}

void optimise_build_allocations(
	const health_wide_t friendly_distribution[model_capacity],
	const health_wide_t enemy_distribution[model_capacity],
	const tile_wide_t buildable_allocations[model_capacity],
	const gold_t budget,
	tile_wide_t build_allocation[model_capacity]) {

	(void)friendly_distribution;
	(void)enemy_distribution;
	(void)build_allocation;

	glp_prob* const problem = glp_create_prob();
	glp_set_prob_name(problem, "build_allocations");
	glp_set_obj_dir(problem, GLP_MAX);
	add_rows(problem, buildable_allocations, budget);

	glp_simplex(problem, NULL);
	(void)problem;
}

