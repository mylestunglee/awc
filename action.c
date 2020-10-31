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
