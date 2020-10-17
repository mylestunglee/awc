#include "bot.h"
#include "bitarray.h"
#include <assert.h>

static void bot_label_unit_target(
	struct game* const game,
	const struct unit* const attacker,
	const struct unit* const attackee)
{
	assert(attacker->player != attackee->player);
	(void)game;
}

static void bot_label_unit_targets(struct game* const game, const struct unit* const unit)
{
	const struct units* const units = &game->units;
	for (player_t player = 0; player < players_capacity; ++player) {
		// Do not label friendly units as targets
		if (bitmatrix_get(game->alliances, game->turn, player))
			continue;

		unit_t curr = units->firsts[player];
		while (curr != null_unit) {
			bot_label_unit_target(game, unit, &units->data[curr]);
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
