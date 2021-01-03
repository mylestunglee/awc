#include <glpk.h>
#include <stdio.h>
#include <assert.h>

#include "optimise.h"
#include "list.h"

#define symbolic_name_length 16
#define sparse_matrix_length (1 + model_capacity + (3 + capturable_capacity) * model_capacity * model_capacity)
#define ranged_inefficiency 0.5

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
	const tile_wide_t capturables[capturable_capacity],
	int* const row_offset) {

	for (tile_t capturable = 0; capturable < capturable_capacity; ++capturable) {
		char name[symbolic_name_length];
		snprintf(name, symbolic_name_length, "v_"tile_format, capturable + 1);
		glp_set_row_name(problem, *row_offset, name);
		glp_set_row_bnds(problem, *row_offset, GLP_UP, 0.0, (double)capturables[capturable]);
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

	glp_add_rows(problem, 1 + capturable_capacity + 2 * model_capacity);

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
			glp_set_col_bnds(problem, *column_offset, GLP_DB, 0.0, 1.0);
			++*column_offset;
		}
	}
}

static void add_allocation_columns(
	glp_prob* const problem,
	const health_wide_t friendly_distribution[model_capacity],
	const health_wide_t enemy_distribution[model_capacity],
	int* const column_offset) {

	for (model_t m = 0; m < model_capacity; ++m) {
		for (model_t n = 0; n < model_capacity; ++n) {
			char name[symbolic_name_length];
			snprintf(name, symbolic_name_length, "y_{"model_format","model_format"}", m + 1, n + 1);
			glp_set_col_name(problem, *column_offset, name);
			glp_set_col_bnds(problem, *column_offset, GLP_LO, 0.0, 0.0);
			glp_set_col_kind(problem, *column_offset, GLP_IV);
			++*column_offset;
		}
	}
}

static void add_ratio_column(glp_prob* const problem, int* const column_offset) {
	glp_set_col_name(problem, *column_offset, "r");
	glp_set_obj_coef(problem, *column_offset, 1.0);
	glp_set_col_bnds(problem, *column_offset, GLP_FR, 0.0, 0.0);
	++*column_offset;
}

static void add_columns(
	glp_prob* const problem,
	const health_wide_t friendly_distribution[model_capacity],
	const health_wide_t enemy_distribution[model_capacity]) {

	glp_add_cols(problem, 1 + 2 * model_capacity * model_capacity);

	int column_offset = 1;
	add_distribution_columns(problem, friendly_distribution, enemy_distribution, &column_offset);
	add_allocation_columns(
		problem,
		friendly_distribution,
		enemy_distribution,
		&column_offset);
	add_ratio_column(problem, &column_offset);
}

struct sparse_matrix {
        int is[sparse_matrix_length];
        int js[sparse_matrix_length];
        double values[sparse_matrix_length];
        unsigned int entries;
};

static void sparse_matrix_set(
	struct sparse_matrix* const matrix,
	const int i,
	const int j,
	const double value) {

	++matrix->entries;
	matrix->is[matrix->entries] = i + 1;
	matrix->js[matrix->entries] = j + 1;
	matrix->values[matrix->entries] = value;
}

static void set_sum_coefficients(
	struct sparse_matrix* const coefficients,
	const int row_offset,
	const int column_offset) {

	for (model_t m = 0; m < model_capacity; ++m)
		for (model_t n = 0; n < model_capacity; ++n) {
			const int i = row_offset + m;
			const int j = column_offset + m * model_capacity + n;
			sparse_matrix_set(coefficients, i, j, 1.0);
		}
}

static void set_capturable_coefficients(
	struct sparse_matrix* const coefficients,
	const int row_offset,
	const int column_offset) {

	for (tile_t capturable = 0; capturable < capturable_capacity; ++capturable)
		for (model_t m = buildable_models[capturable]; m < buildable_models[capturable + 1]; ++m)
			for (model_t n = 0; n < model_capacity; ++n) {
				const int i = row_offset + capturable;
				const int j = column_offset + m * model_capacity + n;
				sparse_matrix_set(coefficients, i, j, 1.0);
			}
}

static void set_cost_coefficients(
	struct sparse_matrix* const coefficients,
	const int row_offset,
	const int column_offset) {

	for (model_t m = 0; m < model_capacity; ++m)
		for (model_t n = 0; n < model_capacity; ++n) {
			const int j = column_offset + m * model_capacity + n;
			sparse_matrix_set(coefficients, row_offset, j, models_cost[m]);
		}
}

static void set_surplus_distribution_coefficients(
	struct sparse_matrix* const coefficients,
	const health_wide_t* const friendly_distribution,
	const health_wide_t enemy_distribution[model_capacity],
	const int row_offset,
	const int column_offset) {

	for (model_t m = 0; m < model_capacity; ++m) {
		if (enemy_distribution[m] == 0)
			continue;

		for (model_t n = 0; n < model_capacity; ++n) {
			const int i = row_offset + m;
			const int j = column_offset + n * model_capacity + m;
			const double value =
				(double)units_damage[n][m] *
				(models_min_range[n] ? ranged_inefficiency : 1.0) *
				(friendly_distribution ? (double)friendly_distribution[n] : 1.0) /
				(double)enemy_distribution[m];
			sparse_matrix_set(coefficients, i, j, value);
		}
	}
}

static void set_surplus_ratio_coefficients(
	struct sparse_matrix* const coefficients,
	const health_wide_t enemy_distribution[model_capacity],
	const int row_offset,
	const int column_offset) {

	for (model_t model = 0; model < model_capacity; ++model) {
		if (enemy_distribution[model] == 0)
			continue;

		sparse_matrix_set(coefficients, row_offset + model, column_offset, -1.0);
	}
}

static void populate_coefficients(
	glp_prob* const problem,
	const health_wide_t friendly_distribution[model_capacity],
	const health_wide_t enemy_distribution[model_capacity],
	struct sparse_matrix* const coefficients) {

	// Reuse game queue as a workspace for storing the coefficient matrix
	assert (sizeof(struct sparse_matrix) < sizeof(struct list_node) * list_capacity);
	coefficients->entries = 0;

	int row_offset = 0;

	set_sum_coefficients(coefficients, row_offset, 0);
	row_offset += model_capacity;

	set_capturable_coefficients(
		coefficients,
		row_offset,
		model_capacity * model_capacity);
	row_offset += capturable_capacity;

	set_cost_coefficients(
		coefficients,
		row_offset,
		model_capacity * model_capacity);
	++row_offset;

	set_surplus_distribution_coefficients(
		coefficients,
		friendly_distribution,
		enemy_distribution,
		row_offset,
		0);

	set_surplus_distribution_coefficients(
		coefficients,
		NULL,
		enemy_distribution,
		row_offset,
		model_capacity * model_capacity);

	set_surplus_ratio_coefficients(
		coefficients,
		enemy_distribution,
		row_offset,
		2 * model_capacity * model_capacity);

	glp_load_matrix(
		problem,
		coefficients->entries,
		coefficients->is,
		coefficients->js,
		coefficients->values);
}

static void populate_build_allocations(
	glp_prob* const problem,
	tile_wide_t build_allocation[model_capacity]) {

	for (model_t m = 0; m < model_capacity; ++m)
		for (model_t n = 0; n < model_capacity; ++n)
			build_allocation[m] += glp_mip_col_val(
				problem,
					1 + n + m * model_capacity + model_capacity * model_capacity);
}

static int solve(glp_prob* const problem) {
	glp_iocp parameters;
	glp_init_iocp(&parameters);
	parameters.presolve = GLP_ON;
	parameters.msg_lev = GLP_MSG_ERR;
	return glp_intopt(problem, &parameters);
}

void optimise_build_allocations(
	const health_wide_t friendly_distribution[model_capacity],
	const health_wide_t enemy_distribution[model_capacity],
	const tile_wide_t capturables[capturable_capacity],
	const gold_t budget,
	tile_wide_t build_allocation[model_capacity],
	void* const workspace) {

	glp_prob* const problem = glp_create_prob();
	glp_set_prob_name(problem, "build_allocations");
	glp_set_obj_dir(problem, GLP_MAX);
	add_rows(problem, capturables, budget);
	add_columns(problem, friendly_distribution, enemy_distribution);
	populate_coefficients(problem, friendly_distribution, enemy_distribution, workspace);

	const int error = solve(problem);

	if (!error)
		populate_build_allocations(problem, build_allocation);

	glp_delete_prob(problem);
}
