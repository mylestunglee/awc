#include "bot.h"
#include "bitarray.h"
#include "grid.h"
#include <assert.h>
#include <stdlib.h>

static void bot_interact_unit(struct game* const game, const struct unit* const unit)
{
	// TODO
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
