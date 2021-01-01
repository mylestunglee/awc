
#include "optimise.h"

#include <glpk.h>
#include <stdio.h>
#define symbolic_name_length 16

static void add_distribution_rows(glp_prob* const problem, int* const row_offset) {
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
	glp_set_row_name(problem, *row_offset, "b");
	glp_set_row_bnds(problem, *row_offset, GLP_UP, 0.0, budget);
	++*row_offset;
}

static void add_surplus_rows(glp_prob* const problem, int* const row_offset) {
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

	glp_add_rows(problem, 1 + 3 * model_capacity);

	int row_offset = 1;
	add_distribution_rows(problem, &row_offset);
	add_allocation_rows(problem, buildable_allocations, &row_offset);
	add_cost_row(problem, budget, &row_offset);
	add_surplus_rows(problem, &row_offset);
}

static void add_distribution_columns(
	glp_prob* const problem,
	const health_wide_t friendly_distribution[model_capacity],
	const health_wide_t enemy_distribution[model_capacity],
	int* const column_offset) {

	for (model_t m = 0; m < model_capacity; ++m) {
		for (model_t n = 0; n < model_capacity; ++n) {
			char name[symbolic_name_length];
			snprintf(name, symbolic_name_length, "x_{"model_format","model_format"}", m + 1, n + 1);
			glp_set_col_name(problem, *column_offset, name);

			if (
				friendly_distribution[m] == 0 ||
				enemy_distribution[n] == 0 ||
				units_damage[m][n] == 0)
				glp_set_col_bnds(problem, *column_offset, GLP_FX, 0.0, 0.0);
			else
				glp_set_col_bnds(problem, *column_offset, GLP_DB, 0.0, 1.0);

			++*column_offset;
		}
	}
}

static void add_allocation_columns(
	glp_prob* const problem,
	const health_wide_t friendly_distribution[model_capacity],
	const health_wide_t enemy_distribution[model_capacity],
	const tile_wide_t buildable_allocations[model_capacity],
	int* const column_offset) {

	for (model_t m = 0; m < model_capacity; ++m) {
		for (model_t n = 0; n < model_capacity; ++n) {
			char name[symbolic_name_length];
			snprintf(name, symbolic_name_length, "y_{"model_format","model_format"}", m + 1, n + 1);
			glp_set_col_name(problem, *column_offset, name);

			if (
				friendly_distribution[m] == 0 ||
				enemy_distribution[n] == 0 ||
				units_damage[m][n] == 0)
				glp_set_col_bnds(problem, *column_offset, GLP_FX, 0.0, 0.0);
			else
				glp_set_col_bnds(problem, *column_offset, GLP_DB, 0.0, (double)buildable_allocations[m]);

			++*column_offset;
		}
	}
}

static void add_columns(
	glp_prob* const problem,
	const health_wide_t friendly_distribution[model_capacity],
	const health_wide_t enemy_distribution[model_capacity],
	const tile_wide_t buildable_allocations[model_capacity]) {


	glp_add_cols(problem, 2 * model_capacity * model_capacity);

	int column_offset = 1;
	add_distribution_columns(problem, friendly_distribution, enemy_distribution, &column_offset);
	add_allocation_columns(
		problem,
		friendly_distribution,
		enemy_distribution,
		buildable_allocations,
		&column_offset);
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
	add_columns(problem, friendly_distribution, enemy_distribution, buildable_allocations);

	glp_simplex(problem, NULL);
	(void)problem;
}

