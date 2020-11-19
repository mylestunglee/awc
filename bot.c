#include "bot.h"
#include "bitarray.h"
#include "grid.h"
#include "game.h"
#include "action.h"
#include <assert.h>
#include <stdlib.h>

static unit_t find_attackee(struct game* const game, const struct unit* const attacker) {
	unit_t best_attackee = null_unit;
	health_wide_t best_metric = 0;

	grid_t y = 0;
	do {
		grid_t x = 0;
		do {
			// Attackee is at an attack-labelled tile
			if (!(game->labels[y][x] & attackable_bit))
				continue;

			game->x = x;
			game->y = y;

			health_t damage, counter_damage;
			game_simulate_attack(game, &damage, &counter_damage);

			const unit_t attackee_index = game->units.grid[y][x];
			const struct unit* const attackee = &game->units.data[attackee_index];
			health_wide_t metric = (health_wide_t)damage * models_cost[attackee->model] -
				(health_wide_t)counter_damage * models_cost[attacker->model];

			if (metric > best_metric || best_attackee == null_unit) {
				best_metric = metric;
				best_attackee = attackee_index;
			}
		} while (++x);
	} while (++y);

	return best_attackee;
}

static void handle_ranged_attack(
	struct game* const game,
	struct unit* const attacker,
	const struct unit* const attackee) {
	game->x = attackee->x;
	game->y = attackee->y;
	action_handle_attack(game);
	attacker->enabled = false;
}

static void handle_direct_attack(
	struct game* const game,
	struct unit* const attacker,
	const struct unit* const attackee) {

	// Find maximal defense tile around attackee
	energy_t max_energy = 0;
	health_t max_defense = 0;
	uint8_t best_i = 0;

	const grid_t x = attackee->x;
	const grid_t y = attackee->y;
	const grid_t adjacent_x[] = {x + 1, x, x - 1, x};
	const grid_t adjacent_y[] = {y, y - 1, y, y + 1};

	// Each tile has four adjacent tiles
	for (uint8_t i = 0; i < 4; ++i) {
		const tile_t i_x = adjacent_x[i];
		const tile_t i_y = adjacent_y[i];

		if (!(game->labels[i_y][i_x] & accessible_bit))
			continue;

		const model_t model = attacker->model;
		const tile_t tile = game->map[i_y][i_x];
		const health_t defense = tile_defense[model][tile];
		const energy_t energy = game->energies[i_y][i_x];

		// Lexicographical ordering over defence then energy
		if (defense > max_defense || (defense == max_defense && energy > max_energy)) {
			max_defense = defense;
			max_energy = energy;
			best_i = i;
		}
	}

	assert (max_energy > 0);

	// Apply attack
	game->x = x;
	game->y = y;
	game->prev_x = adjacent_x[best_i];
	game->prev_y = adjacent_y[best_i];
	action_handle_attack(game);
	attacker->enabled = false;
}

static void handle_attack(struct game* const game, struct unit* const attacker) {
	const unit_t attackee_index = find_attackee(game, attacker);

	if (attackee_index == null_unit)
		return;

	const struct unit* const attackee = &game->units.data[attackee_index];

	if (models_min_range[attacker->model])
		handle_ranged_attack(game, attacker, attackee);
	else
		handle_direct_attack(game, attacker, attackee);
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

			if (energy > max_energy) {
				max_energy = energy;
				*nearest_x = x;
				*nearest_y = y;
			}
		} while (++x);
	} while (++y);

	return max_energy > 0;
}

// Capture nearest enemy capturable
static void handle_capture(struct game* const game, struct unit* const unit) {
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
static void handle_local(struct game* const game, struct unit* const unit) {
	assert (unit->enabled);

	// Populate labels and workspace
	game->x = unit->x;
	game->y = unit->y;

	// Scan for local targets
	grid_explore(game, false);
	handle_attack(game, unit);

	if (unit->enabled)
		handle_capture(game, unit);

}

// TODO: may not be required if use bot_find_closest_capturable
static void mark_capturable_tiles(struct game* const game, const struct unit* const unit) {
	grid_t y = 0;
	do {
		grid_t x = 0;
		do {
			bool accessible = game->energies[y][x] > 0;

			bool tile_capturable = game->map[y][x] >= tile_capturable_lower_bound;
			bool tile_unfriendly = !bitmatrix_get(game->alliances, game->territory[y][x], game->turn);
			bool unit_capturable = unit->model < unit_capturable_upper_bound;
			bool capturable = tile_capturable && tile_unfriendly && unit_capturable;

			if (accessible && capturable)
				game->labels[y][x] |= bot_target_bit;
		} while (++x);
	} while (++y);
}

// TODO: implement bot_find_closest_attackable
static void mark_attackable_tiles(struct game* const game, const struct unit* const unit) {
	(void)game;
	(void)unit;

	// for each enemy player
		// for each attackable unit
			// if ranged
				// loop diamond around unit
			// else
				// mark adjacent tiles
}

static void handle_nonlocal(struct game* const game, struct unit* const unit) {
	// Number of turns of unit movement to look ahead
	const energy_t look_ahead = 4;

	assert(game->x == unit->x);
	assert(game->y == unit->y);

	// label_attackable_tiles argument=false is unimportant because attack_bit is unread
	grid_explore_recursive(game, false, look_ahead);
	mark_capturable_tiles(game, unit);
	mark_attackable_tiles(game, unit);
	// find closest target; go to such target

	grid_clear_uint8(game->labels);
	grid_clear_energy(game->energies);
}

static void interact_unit(struct game* const game, struct unit* const unit) {
	assert (game->turn == unit->player);
	game->selected = game->units.grid[unit->y][unit->x];
	handle_local(game, unit);

	if (unit->enabled)
		handle_nonlocal(game, unit);

	game->selected = null_unit;
}

static void interact_units(struct game* const game) {
	assert (game->selected == null_unit);

	struct units* const units = &game->units;
	unit_t curr = units->firsts[game->turn];
	while (curr != null_unit) {
		interact_unit(game, &units->data[curr]);
		curr = units->nexts[curr];
	}

}

void bot_play(struct game* const game)
{
	interact_units(game);
}
