#ifndef asserts_h
#define asserts_h

#include <stdio.h>
#include "definitions.h"

#define compiler_assert(EXPRESSION)   switch (0) {case 0: case (EXPRESSION):;}
#define is_unsigned(type) ((type)(-1) > (type) 0)
#define max_value(type) (type)~(type)0
#define upper_bound(type) max_value(type) + 1

void asserts() {
	// Assert signness of type definitions
	compiler_assert(is_unsigned(unit_t));
	compiler_assert(is_unsigned(health_t));
	compiler_assert(is_unsigned(health_wide_t));
	compiler_assert(is_unsigned(model_t));
	compiler_assert(is_unsigned(grid_t));
	compiler_assert(!is_unsigned(grid_wide_t));
	compiler_assert(is_unsigned(queue_t));
	compiler_assert(is_unsigned(energy_t));
	compiler_assert(is_unsigned(energy_t));
	compiler_assert(is_unsigned(tile_t));
	compiler_assert(is_unsigned(player_t));
	compiler_assert(is_unsigned(player_wide_t));
	compiler_assert(!is_unsigned(gold_t));

	// Assert wide types
	compiler_assert(4 * sizeof(health_t) == sizeof(health_wide_t));
	compiler_assert(2 * sizeof(grid_t) == sizeof(grid_wide_t));
	compiler_assert(2 * sizeof(player_t) == sizeof(player_wide_t));

	// Assert type max values compiler_assert(grid_size == upper_bound(grid_t));
	compiler_assert(health_max <= max_value(health_t));
	compiler_assert(tile_capacity <= upper_bound(tile_t));
	compiler_assert(model_capacity <= upper_bound(model_t));
	compiler_assert(queue_capacity == max_value(queue_t));
	compiler_assert(units_capacity == max_value(unit_t));
	compiler_assert(null_unit == units_capacity);
	compiler_assert(null_player == players_capacity);
	compiler_assert(players_capacity <= max_value(player_t));

	// Assert type formats are compatible
	printf(unit_format, (unit_t)0);
	printf(grid_format, (grid_t)0);
	printf(player_format, (health_t)0);
	printf(row_format, "");
	printf(model_format, (model_t)0);
	printf(model_name_format, "");
	printf(turn_format, (player_t)0);
	printf(health_wide_format, 0);
	printf(gold_format, (gold_t)0);

	// Assert graphics
	compiler_assert(screen_width < max_value(grid_t));
	compiler_assert(screen_height < max_value(grid_t));
	compiler_assert(0 <= unit_left);
	compiler_assert(unit_left + unit_width < tile_width);
	compiler_assert(0 <= unit_top);
	compiler_assert(unit_top + unit_height < tile_height);
}

#undef compiler_assert
#undef is_signed
#undef max_value
#undef upper_bound

#endif
