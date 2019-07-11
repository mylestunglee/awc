#include <stdio.h>
#include <string.h>
#include "file.h"
#include "units.h"

bool file_load(struct game* const game, const char* const filename) {
	FILE* const file = fopen(filename, "r");

	const char* delim = " ";
	const int buffer_size = 4096;
	char line[buffer_size];

	while (fgets(line, buffer_size, file)) {
		char* saveptr;
		char* key = __strtok_r(line, delim, &saveptr);

		if (strcmp(key, "turn") == 0) {
			player_t turn;
			sscanf(saveptr, "%hhu", &turn);
			game->turn = turn - 1;
		} else if (strcmp(key, "map") == 0) {
			grid_t y;
			char map_str[grid_size] = {0};
			sscanf(saveptr, "%hhu%s", &y, map_str);
			--y;
			grid_t x = 0;
			while (map_str[x] != '\0') {
				for (tile_t tile = 0; tile < tile_capacity; ++tile) {
					if (map_str[x] == tile_symbols[tile])
						game->map[y][x] = tile;
				}
				++x;
			}
		} else if (strcmp(key, "territory") == 0) {
			grid_t x, y;
			player_t player;
			sscanf(saveptr, "%hhu%hhu%hhu", &player, &x, &y);
			--player;
			--x;
			--y;
			game->territory[y][x] = player;
		} else {
			for (model_t model = 0; model < model_capacity; ++model) {
				if (strcmp(key, model_names[model]) == 0) {
					grid_t x, y;
					player_t player;
					health_t_wide health;
					sscanf(saveptr, "%hhu%hhu%hhu%u", &player, &x, &y, &health);
					--player;
					--x;
					--y;
					units_insert(&game->units, (struct unit){
						.model = model,
						.x = x,
						.y = y,
						.health = (health * (health_max + 1) / 1000) - 1,
						.player = player});
				}
			}
		}

		printf("%s\n", key);
	}

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
