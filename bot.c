#include "bot.h"
#include "bitarray.h"
#include "grid.h"
#include <assert.h>
#include <stdlib.h>

static bool bot_in_alliance(
	const struct game* const game,
	const player_t attacker,
	const player_t attackee)
{
	return bitmatrix_get(game->alliances, attacker, attackee);
}

static void bot_label_unit_target_unit(
	struct game* const game,
	const struct unit* const attacker,
	const struct unit* const attackee)
{
	assert(attacker->player != attackee->player);

	const model_t model = attacker->model;
	const grid_t min_range = models_min_range[model];
	const grid_t max_range = models_max_range[model];
	const grid_t x = attacker->x;
	const grid_t y = attacker->y;

	// Handle direct attacking unit
	if (!min_range) {
		game->labels[y][x - 1] |= target_bit;
		game->labels[y][x + 1] |= target_bit;
		game->labels[y - 1][x] |= target_bit;
		game->labels[y + 1][x] |= target_bit;
		return;
	}

	// Handle ranged attacking unit
	for (grid_wide_t j = -max_range; j <= max_range; ++j)
		for (grid_wide_t i = -max_range; i <= max_range; ++i) {
			const grid_wide_t distance = abs(i) + abs(j);

			if (min_range <= distance && distance <= max_range)
				game->labels[(grid_t)(y + j)][(grid_t)(x + i)] |= target_bit;
		}
}

static void bot_label_unit_target_territory(struct game* const game, const struct unit* const unit)
{
	if (unit->model >= unit_capturable_upper_bound)
		return;

	grid_t y = 0;
	do {
		grid_t x = 0;
		do {
			if (!bot_in_alliance(game, unit->player, game->territory[y][x]))
				game->labels[y][x] |= target_bit;
		} while (++x);
	} while (++y);
}

static void bot_label_unit_targets(struct game* const game, const struct unit* const unit)
{
	const struct units* const units = &game->units;
	for (player_t player = 0; player < players_capacity; ++player) {
		// Do not label friendly units as targets
		if (bot_in_alliance(game, unit->player, player))
			continue;

		bot_label_unit_target_territory(game, unit);

		unit_t curr = units->firsts[player];
		while (curr != null_unit) {
			bot_label_unit_target_unit(game, unit, &units->data[curr]);
			curr = units->nexts[curr];
		}
	}
}

static void bot_interact_unit(struct game* const game, const struct unit* const unit)
{
	// Assume labels and workspace are clean
	// Label target tiles
	bot_label_unit_targets(game, unit);
	// Path find to a target
	// Move unit towards target

	// cleanup
	grid_clear_all_uint8(game->labels);
}

static void bot_interact_units(struct game* const game)
{
	const struct units* const units = &game->units;
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
