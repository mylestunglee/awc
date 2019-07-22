#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "file.h"
#include "units.h"
#include "bitarray.h"

static void file_load_turn(struct game* const game, const char* const tokens) {
	player_t turn;
	if (sscanf(tokens, "%hhu", &turn) != 1)
		return;

	game->turn = turn - 1;
}

static void file_load_map(struct game* const game, const char* const tokens) {
	grid_t y;
	char map_str[grid_size] = {0};
	if (sscanf(tokens, grid_format row_format, &y, map_str) != 2)
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

	if (sscanf(tokens, player_format grid_format grid_format, &player, &x, &y) != 3)
		return;

	--player;
	--x;
	--y;

	if (player < players_capacity)
		game->territory[y][x] = player;
}

static void file_load_bot(struct game* const game, const char* const tokens) {
	player_t player;
	if (sscanf(tokens, player_format, &player) != 1)
		return;

	--player;

	if (player < players_capacity)
		bitarray_set(game->bots, player);
}

static void file_load_unit(struct game* const game, const char* const tokens, const model_t model) {
	grid_t x, y;
	player_t player;
	health_wide_t health;
	char enabled[8];
	if (sscanf(tokens,
			player_format
			grid_format
			grid_format
			health_wide_format
			"%8s", &player, &x, &y, &health, enabled) != 5)
		return;

	--player;
	--x;
	--y;

	if (player < players_capacity)
		units_insert(&game->units, (struct unit){
			.model = model,
			.x = x,
			.y = y,
			.health = health,
			.player = player,
			.enabled = !strcmp(enabled, "enabled")});
}

static void file_load_gold(struct game* const game, const char* const tokens) {
	player_t player;
	gold_t gold;
	if (sscanf(tokens, player_format gold_format, &player, &gold) != 2)
		return;

	--player;

	if (player < players_capacity)
		game->golds[player] = gold;
}

bool file_load(struct game* const game, const char* const filename) {
	FILE* const file = fopen(filename, "r");

	if (!file)
		return true;

	const char* delim = " ";
	const uint16_t buffer_size = 4096;
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
		else if (!strcmp(key, "gold"))
			file_load_gold(game, tokens);
		else if (!strcmp(key, "bot"))
			file_load_bot(game, tokens);
		else if (!strcmp(key, "fog\n"))
			game->fog = true;
		else
			for (model_t model = 0; model < model_capacity; ++model)
				if (!strcmp(key, model_names[model])) {
					file_load_unit(game, tokens, model);
					break;
				}
	}

	return fclose(file) < 0;
}

static grid_wide_t file_row_length(const struct game* const game, const grid_t y) {
	for (grid_wide_t x = grid_size - 1; x >= 0; --x) {
		if (game->map[y][x]) {
			return x;
		}
	}
	return -1;
}

static void file_save_map(const struct game* const game, FILE* const file) {
	grid_t y = 0;
	do {
		const grid_wide_t length = file_row_length(game, y);

		if (length < 0)
			continue;

		fprintf(file, "map "grid_format" ", y + 1);

		for (grid_wide_t x = 0; x <= length; ++x) {
			fprintf(file, "%c", tile_symbols[game->map[y][x]]);
		}
		fprintf(file, "\n");
	} while (++y);
}

static void file_save_units(const struct game* const game, FILE* const file) {
	for (player_t player = 0; player < players_capacity; ++player) {
		unit_t curr = game->units.firsts[player];
		while (curr != null_unit) {
			const struct unit* const unit = &game->units.data[curr];
			fprintf(file,
				model_format" "
				player_format" "
				grid_format" "
				grid_format" "
				health_format" %s\n",
				model_names[unit->model],
				unit->player + 1,
				unit->x + 1,
				unit->y + 1,
				unit->health,
				unit->enabled ? "enabled" : "disabled");
			curr = game->units.nexts[curr];
		}
	}
}

static void file_save_territory(const struct game* const game, FILE* const file) {
	grid_t y = 0;
	do {
		grid_t x = 0;
		do {
			if (game->territory[y][x] != null_player)
				fprintf(file, "territory "player_format" "grid_format" "grid_format"\n", game->territory[y][x] + 1, x + 1, y + 1);
		} while (++x);
	} while (++y);
}

static void file_save_golds(const struct game* const game, FILE* const file) {
	for (player_t player = 0; player < players_capacity; ++player)
		if (game->golds[player])
			fprintf(file, "gold "player_format" "gold_format"\n", player + 1, game->golds[player]);
}

static void file_save_bots(const struct game* const game, FILE* const file) {
	for (player_t player = 0; player < players_capacity; ++player)
		if (bitarray_get(game->bots, player))
			fprintf(file, "bot "player_format"\n", player + 1);
}

bool file_save(const struct game* const game, const char* const filename) {
	FILE* const file = fopen(filename, "w");

	if (!file)
		return true;

	fprintf(file, "turn "turn_format"\n", game->turn + 1);
	file_save_map(game, file);
	file_save_units(game, file);
	file_save_territory(game, file);
	file_save_golds(game, file);
	file_save_bots(game, file);
	if (game->fog)
		fprintf(file, "fog");

	return fclose(file) < 0;
}
