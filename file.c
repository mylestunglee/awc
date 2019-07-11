#include <stdio.h>
#include "file.h"

bool file_load(const struct game* const game, const char* const filename) {
	return false;
}

static int file_row_length(const struct game* const game, const grid_t y) {
	for (int x = grid_size - 1; x >= 0; --x) {
		if (game->map[y][x] != 0) {
			return x;
		}
	}
	return -1;
}

static void file_save_map(const struct game* const game, FILE* const file) {
	grid_t y = 0;
	do {
		int length = file_row_length(game, y);

		if (length < 0)
			continue;

		fprintf(file, "map "grid_t_format" ", y + 1);

		for (int x = 0; x <= length; ++x) {
			fprintf(file, "%c", tile_symbols[game->map[y][x]]);
		}
		fprintf(file, "\n");
	} while (++y);
}

bool file_save(const struct game* const game, const char* const filename) {
	FILE* const file = fopen(filename, "w");

	if (!file)
		return true;

	// Write turn
	fprintf(file, "turn %u\n", game->turn + 1);

	file_save_map(game, file);

	// Write units
	for (player_t player = 0; player < players_capacity; ++player) {
		unit_t curr = game->units.firsts[player];
		while (curr != null_unit) {
			const struct unit* const unit = &game->units.data[curr];
			fprintf(file, "%-12s "player_format" "grid_t_format" "grid_t_format" "health_format"\n", model_names[unit->model], unit->player + 1, unit->x + 1, unit->y + 1, 1000 * (unit->health + 1) / (health_max + 1));
			curr = game->units.nexts[curr];
		}
	}

	// Write territory
	{
		grid_t y = 0;
		do {
			grid_t x = 0;
			do {
				if (game->territory[y][x] != null_player)
					fprintf(file, "territory "player_format" "grid_t_format" "grid_t_format"\n", game->territory[y][x] + 1, x + 1, y + 1);
			} while (++x);
		} while (++y);
	}
	fclose(file);

	return false;
}
