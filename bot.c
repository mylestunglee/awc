#include "bot.h"
#include "bitarray.h"
#include "grid.h"
#include "game.h"
#include "action.h"
#include "optimise.h"
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
	action_attack(game);
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
		const grid_t i_x = adjacent_x[i];
		const grid_t i_y = adjacent_y[i];

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
	action_attack(game);
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

static void update_max_energy(
	const struct game* const game,
	grid_t x,
	grid_t y,
	energy_t* const max_energy,
	grid_t* const update_x,
	grid_t* const update_y) {

	const energy_t energy = game->energies[y][x];

	if (energy <= *max_energy)
		return;

	*max_energy = energy;
	*update_x = x;
	*update_y = y;
}

static bool is_friendly(const struct game* const game, const player_t player) {
	return bitmatrix_get(game->alliances, game->turn, player);
}

static energy_t find_nearest_capturable(
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
			if (game->map[y][x] < terrian_capacity)
				continue;

			if (is_friendly(game, game->territory[y][x]))
				continue;

			update_max_energy(game, x, y, &max_energy, nearest_x, nearest_y);
		} while (++x);
	} while (++y);

	return max_energy;
}

// Capture nearest enemy capturable
static void handle_capture(struct game* const game, struct unit* const unit) {
	assert (unit->enabled);

	// Unit can capture
	if (unit->model >= unit_capturable_upper_bound)
		return;

	grid_t x, y;
	const energy_t found = find_nearest_capturable(game, &x, &y);

	if (found == 0)
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

	grid_clear_uint8(game->labels);
	grid_clear_energy(game->energies);
}

static void find_nearest_attackee_target_ranged(
	const struct game* const game,
	const struct unit* const attacker,
	const struct unit* const attackee,
	energy_t* const max_energy,
	grid_t* const nearest_x,
	grid_t* const nearest_y) {

	const model_t model = attacker->model;
	const grid_wide_t max_range = models_max_range[model];

	for (grid_wide_t j = -max_range; j <= max_range; ++j)
		for (grid_wide_t i = -max_range; i <= max_range; ++i) {
			const grid_wide_t distance = abs(i) + abs(j);
			if (models_min_range[model] <= distance && distance <= max_range)
				update_max_energy(
					game,
					(grid_wide_t)(attackee->x) + i,
					(grid_wide_t)(attackee->y) + j,
					max_energy,
					nearest_x,
					nearest_y);
		}
}

static void find_nearest_attackee_target_direct(
	const struct game* const game,
	const struct unit* const attackee,
	energy_t* const max_energy,
	grid_t* const nearest_x,
	grid_t* const nearest_y) {

	const grid_t x = attackee->x;
	const grid_t y = attackee->y;
	const grid_t adjacent_x[] = {x + 1, x, x - 1, x};
	const grid_t adjacent_y[] = {y, y - 1, y, y + 1};

	for (uint8_t i = 0; i < 4; ++i)
		update_max_energy(
			game,
			adjacent_x[i],
			adjacent_y[i],
			max_energy,
			nearest_x,
			nearest_y);
}

static energy_t find_nearest_attackee_target(
	struct game* const game,
	const struct unit* const attacker,
	grid_t* const nearest_x,
	grid_t* const nearest_y) {

	// Maximise remaining energy to find nearest
	energy_t max_energy = 0;

	for (player_t player = 0; player < players_capacity; ++player) {

		if (is_friendly(game, player))
			continue;

		unit_t curr = game->units.firsts[player];
		while (curr != null_unit) {
			const struct unit* const attackee = &game->units.data[curr];

			// Attackee is attackable
			if (units_damage[attacker->model][attackee->model] > 0) {
				// If attacker is ranged
				if (models_min_range[attacker->model])
					find_nearest_attackee_target_ranged(
						game,
						attacker,
						attackee,
						&max_energy,
						nearest_x,
						nearest_y);
				else
					find_nearest_attackee_target_direct(
						game,
						attackee,
						&max_energy,
						nearest_x,
						nearest_y);
			}

			curr = game->units.nexts[curr];
		}
	}

	return max_energy;
}

static bool find_nearest_target(
	struct game* const game,
	struct unit* const unit,
	grid_t* const nearest_x,
	grid_t* const nearest_y) {

	grid_t attackee_target_x, attackee_target_y, capturable_x, capturable_y;
	const energy_t attackee_target_energy = find_nearest_attackee_target(
		game,
		unit,
		&attackee_target_x,
		&attackee_target_y);

	energy_t capturable_energy = 0;

	if (unit->model < unit_capturable_upper_bound)
		find_nearest_capturable(
			game,
			&capturable_x,
			&capturable_y);

	if (attackee_target_energy > capturable_energy) {
		*nearest_x = attackee_target_x;
		*nearest_y = attackee_target_y;
	} else {
		*nearest_x = capturable_x;
		*nearest_y = capturable_y;
	}

	return attackee_target_energy > 0 || capturable_energy > 0;
}

static void move_towards_target(
	struct game* const game,
	struct unit* const unit,
	grid_t x,
	grid_t y) {

	grid_find_path(game, x, y);

	struct list* const list = &game->list;

	assert (!list_empty(list));

	const energy_t accessible_energy = list_back_peek(list).energy - unit_movement_ranges[unit->model];

	while (!list_empty(list) && list_back_peek(list).energy >= accessible_energy) {
		const struct list_node node = list_back_pop(list);
		x = node.x;
		y = node.y;
	}

	list_initialise(list);
	units_move(&game->units, game->units.grid[unit->y][unit->x], x, y);
	unit->enabled = false;
}


static void handle_nonlocal(struct game* const game, struct unit* const unit) {
	// Number of turns of unit movement to look ahead
	const energy_t look_ahead = 16;

	assert(game->x == unit->x);
	assert(game->y == unit->y);

	// label_attackable_tiles argument=false is unimportant because attack_bit is unread
	grid_explore_recursive(game, false, look_ahead);
	grid_t x, y;
	bool found = find_nearest_target(game, unit, &x, &y);

	if (found)
		move_towards_target(game, unit, x, y);

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

static void accumulate_distribution(
	const struct game* const game,
	const player_t player,
	health_wide_t distribution[model_capacity]) {

	const struct units* const units = &game->units;
	unit_t curr = units->firsts[player];
	while (curr != null_unit) {
		const struct unit* const unit = &units->data[curr];
		distribution[unit->model] += unit->health;
		curr = units->nexts[curr];
	}
}

static void populate_distributions(
	const struct game* const game,
	health_wide_t friendly_distribution[model_capacity],
	health_wide_t enemy_distribution[model_capacity]) {

	for (player_t player = 0; player < players_capacity; ++player) {
		if (is_friendly(game, player))
			accumulate_distribution(game, player, friendly_distribution);
		else
			accumulate_distribution(game, player, enemy_distribution);
	}
}

static bool is_nonzero_distribution(const health_wide_t distribution[model_capacity]) {
	for (model_t model = 0; model < model_capacity; ++model)
		if (distribution[model])
			return true;

	return false;
}


static void populate_capturables(
	const struct game* const game,
	tile_wide_t capturables[capturable_capacity]) {

	grid_t y = 0;
	do {
		grid_t x = 0;
		do {
			if (game->territory[y][x] != game->turn)
				continue;

			const tile_t capturable = game->map[y][x] - terrian_capacity;
			++capturables[capturable];
		} while (++x);
	} while (++y);
}

// Maximise infantry build allocations
static void default_build_allocations(
	const tile_wide_t capturables[capturable_capacity],
	const gold_t budget,
	tile_wide_t build_allocations[model_capacity]) {

	// Find infantry-buildable capturable
	tile_t capturable = 0;
	while (capturable < capturable_capacity &&
		buildable_models[capturable + 1] == 0) {

		++capturable;
	}
	assert (capturable < capturable_capacity);

	// Populate build_allocations
	const tile_wide_t budget_allocatable = budget / models_cost[0];
	const tile_wide_t capturable_allocatable = capturables[capturable];

	if (budget_allocatable < capturable_allocatable)
		build_allocations[0] = budget_allocatable;
	else
		build_allocations[0] = capturable_allocatable;
}

static void realise_build_allocations(
	struct game* const game,
	tile_wide_t build_allocations[model_capacity]) {

	grid_t y = 0;
	do {
		grid_t x = 0;
		do {
			if (game->territory[y][x] != game->turn)
				continue;

			if (game->units.grid[y][x] != null_unit)
				continue;

			const tile_t capturable = game->map[y][x] - terrian_capacity;

			for (model_t model = buildable_models[capturable]; model < buildable_models[capturable + 1]; ++model) {
				if (build_allocations[model] == 0)
					continue;

				game->x = x;
				game->y = y;
				bool error = action_build(game, model);

				if (error)
					return;

				--build_allocations[model];
				break;
			}
		} while (++x);
	} while (++y);
}

static void build_units(struct game* const game) {
	if (capturable_capacity == 0)
		return;

	// Prepare build allocation decision structures
	health_wide_t friendly_distribution[model_capacity] = {0};
	health_wide_t enemy_distribution[model_capacity] = {0};
	populate_distributions(game, friendly_distribution, enemy_distribution);
	tile_wide_t capturables[capturable_capacity] = {0};
	populate_capturables(game, capturables);
	tile_wide_t build_allocations[model_capacity] = {0};

	// Perform build allocation decision
	if (is_nonzero_distribution(enemy_distribution))
		optimise_build_allocations(
			friendly_distribution,
			enemy_distribution,
			capturables,
			game->golds[game->turn],
			build_allocations,
			&game->list.nodes);
	else
		default_build_allocations(
			capturables,
			game->golds[game->turn],
			build_allocations);

	realise_build_allocations(game, build_allocations);
}

void bot_play(struct game* const game) {
	// Clear previous user interaction
	game->selected = null_unit;
	grid_clear_uint8(game->labels);

	interact_units(game);
	build_units(game);
}
