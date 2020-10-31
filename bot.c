#include "bot.h"
#include "bitarray.h"
#include "grid.h"
#include "action.h"
#include <assert.h>
#include <stdlib.h>

static void bot_interact_unit_ranged(struct game* const game, const struct unit* const unit)
{
	(void)game;
	(void)unit;
}

static void bot_interact_unit_direct(struct game* const game, const struct unit* const unit)
{
	(void)game;
	(void)unit;
}

static bool bot_find_nearest_capturable(
	struct game* const game,
	grid_t* const nearest_x,
	grid_t* const nearest_y)
{
	// Maximise remaining energy to find nearest
	energy_t max_energy = 0;
	grid_t y = 0;
	do {
		grid_t x = 0;
		do {
			// Tile is capturable
			if (game->map[y][x] < tile_capturable_lower_bound)
				continue;

			const energy_t energy = game->energies[y][x];

			// Unit can move within range
			if (energy == 0)
				continue;

			const bool friendly = bitmatrix_get(
				game->alliances,
				game->territory[y][x],
				game->turn);

			if (friendly)
				continue;

			if (energy > max_energy)
			{
				max_energy = energy;
				*nearest_x = x;
				*nearest_y = y;
			}
		} while (++x);
	} while (++y);

	return max_energy > 0;
}

// Capture nearest enemy capturable
static void bot_interact_unit_capture(struct game* const game, struct unit* const unit)
{
	assert (unit->enabled);

	// Unit can capture
	if (unit->model >= unit_capturable_upper_bound)
		return;

	grid_t x, y;
	const bool found = bot_find_nearest_capturable(game, &x, &y);

	if (!found)
		return;

	game->x = x;
	game->y = y;
	units_move(&game->units, game->units.grid[unit->y][unit->x], x, y);
	action_handle_capture(game);
	assert (game->territory[y][x] == unit->player);
	unit->enabled = false;
}

// Attempt single-turn operation
static void bot_interact_unit_trivial(struct game* const game, struct unit* const unit)
{
	// Populate labels and workspace
	game->x = unit->x;
	game->y = unit->y;
	grid_explore(game, false);

	// Scan for something to do
	assert (unit->enabled);

	if (unit->enabled)
		bot_interact_unit_capture(game, unit);

	grid_clear_uint8(game->labels);
	grid_clear_energy(game->energies);
}

static void bot_interact_unit(struct game* const game, struct unit* const unit)
{
	assert (game->turn == unit->player);

	bot_interact_unit_trivial(game, unit);

	if (!unit->enabled)
		return;

	if (models_min_range[unit->model]) {
		bot_interact_unit_ranged(game, unit);
	} else {
		bot_interact_unit_direct(game, unit);
	}
}

static void bot_interact_units(struct game* const game)
{
	struct units* const units = &game->units;
	unit_t curr = units->firsts[game->turn];
	while (curr != null_unit) {
		bot_interact_unit(game, &units->data[curr]);
		curr = units->nexts[curr];
	}
}

void bot_play(struct game* const game)
{
	bot_interact_units(game);
}
