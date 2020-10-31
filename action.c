#include "action.h"
#include "grid.h"
#include "bitarray.h"
#include <assert.h>

// Occurs when unit captures enemy capturable
static void action_capture(struct game* const game) {
	const player_t loser = game->territory[game->y][game->x];

	// If the enemy loses their HQ
	if (game->map[game->y][game->x] == tile_HQ) {
		assert(loser != null_player);
		assert(loser != game->turn);

		// Remove units
		units_delete_player(&game->units, loser);

		// HQ must be owned
		assert(loser != null_player);
		assert(loser != game->turn);

		// Remove territory
		grid_clear_player_territory(game->territory, loser);

		// Change HQ into a city
		game->map[game->y][game->x] = tile_city;

		game->incomes[loser] = 0;
	} else if (loser != null_player)
		--game->incomes[loser];

	// Reveal does not need to be set because a unit is assumed to be capturing locally
	if (loser != game->turn) {
		game->territory[game->y][game->x] = game->turn;
		++game->incomes[game->turn];
	}
}

void action_handle_capture(struct game* const game)
{
	assert (game->selected != null_unit);

	const struct unit* const unit = &game->units.data[game->selected];

	assert (unit->player == game->turn);
	assert (unit->x == game->x);
	assert (unit->y == game->y);

	// The moved unit can capture iff:
	// 1. The unit is a infantry or a mech
	// 2. The tile is capturable
	// 3. The tile is owned by an enemy
	if (unit->model >= unit_capturable_upper_bound)
		return;

	if (game->map[game->y][game->x] < tile_capturable_lower_bound)
		return;


	if (bitmatrix_get(
			game->alliances,
			game->territory[game->y][game->x],
			unit->player))
		return;

	action_capture(game);
}

void action_handle_attack(struct game* const game) {
	assert(game->units.grid[game->y][game->x] != null_unit);
	assert(game->selected != null_unit);

	struct unit* const attacker = &game->units.data[game->selected];
	const unit_t attackee_index = game->units.grid[game->y][game->x];
	struct unit* const attackee = &game->units.data[attackee_index];
	// Skip to post-attack cleanup when a unit dies
	do {
		// If unit is direct, move to attack
		const bool ranged = models_min_range[attacker->model];
		if (!ranged)
			units_move(&game->units, game->selected, game->prev_x, game->prev_y);

		// Compute damage
		health_t damage, counter_damage;
		game_simulate_attack(game, &damage, &counter_damage);

		// Apply damage
		if (damage > attackee->health) {
			units_delete(&game->units, attackee_index);
			break;
		}

		attackee->health -= damage;

		// Ranged units do not receive counter-attacks
		if (ranged)
			break;

		// Apply counter damage
		if (counter_damage > attacker->health) {
			units_delete(&game->units, game->selected);
			break;
		}

		attacker->health -= counter_damage;
	} while (false);
}

