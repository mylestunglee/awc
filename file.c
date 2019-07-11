#include <stdio.h>
#include "file.h"

bool file_load(const struct game* const game, const uint8_t* const filename) {
	return false;
}

bool file_save(const struct game* const game, const uint8_t* const filename) {
	FILE * const file = fopen((const char*)filename, "w");

	if (!file)
		return true;

	// Write turn
	fprintf(file, "turn %u\n", game->turn + 1);

	// Write map
	grid_t y = 0;
	do {
		fprintf(file, "map "grid_t_format" ", y + 1);
		grid_t x = 0;
		do {
			fprintf(file, "%c", tile_symbols[game->map[y][x]]);
		} while (++x);
		fprintf(file, "\n");
	} while (++y);

	// Write units
	for (player_t player = 0; player < players_capacity; ++player) {
		unit_t curr = game->units.firsts[player];
		while (curr != null_unit) {
			const struct unit* const unit = &game->units.data[curr];
			fprintf(file, "unit "player_format" "grid_t_format" "grid_t_format" "health_format"\n", unit->player, unit->x, unit->y, unit->health);
			curr = game->units.nexts[curr];
		}
	}

	fclose(file);

	return false;
}
