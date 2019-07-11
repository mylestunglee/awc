#include <stdio.h>
#include <string.h>
#include "file.h"
#include "units.h"

static void file_load_turn(struct game* const game, const char* const tokens) {
	player_t turn;
	if (sscanf(tokens, "%hhu", &turn) != 1)
		return;

	game->turn = turn - 1;
}

static void file_load_map(struct game* const game, const char* const tokens) {
	grid_t y;
	char map_str[grid_size] = {0};
	if (sscanf(tokens, "%hhu%s", &y, map_str) != 2)
		return;

	--y;
	grid_t x = 0;

	// Read symbols left to right
	while (map_str[x] != '\0') {
		// Find tile index for symbol
		for (tile_t tile = 0; tile < tile_capacity; ++tile) {
			if (map_str[x] == tile_symbols[tile]) {
				game->map[y][x] = tile;
				break;
			}
		}
		++x;
	}
}

static void file_load_territory(struct game* const game, const char* const tokens) {
	grid_t x, y;
	player_t player;

	if (sscanf(tokens, "%hhu%hhu%hhu", &player, &x, &y) != 3)
		return;

	--player;
	--x;
	--y;

	if (player < players_capacity)
		game->territory[y][x] = player;
}

static void file_load_unit(struct game* const game, const char* const tokens, const model_t model) {
	grid_t x, y;
	player_t player;
	health_t_wide health;
	if (sscanf(tokens, "%hhu%hhu%hhu%u", &player, &x, &y, &health) != 4)
		return;

	--player;
	--x;
	--y;

	if (player < players_capacity)
		units_insert(&game->units, (struct unit){
			.model = model,
			.x = x,
			.y = y,
			.health = (health * (health_max + 1) / 1000) - 1,
			.player = player});
}

bool file_load(struct game* const game, const char* const filename) {
	FILE* const file = fopen(filename, "r");

	const char* delim = " ";
	const int buffer_size = 4096;
	char line[buffer_size];

	while (fgets(line, buffer_size, file)) {
		char* tokens;
		char* key = __strtok_r(line, delim, &tokens);

		if (!strcmp(key, "turn"))
			file_load_turn(game, tokens);
		else if (!strcmp(key, "map"))
			file_load_map(game, tokens);
		else if (!strcmp(key, "territory"))
			file_load_territory(game, tokens);
		else
			for (model_t model = 0; model < model_capacity; ++model)
				if (!strcmp(key, model_names[model])) {
					file_load_unit(game, tokens, model);
					break;
				}
	}

	fclose(file);

	return false;
}

static int file_row_length(const struct game* const game, const grid_t y) {
	for (int x = grid_size - 1; x >= 0; --x) {
		if (game->map[y][x]) {
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
