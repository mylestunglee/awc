#include <stdio.h>
#include "console.h"
#include "game.h"
#include "graphics.h"
#include "grid.h"
#include "file.h"

static void game_territory_initialise(player_t territory[grid_size][grid_size]) {
	grid_t y = 0;
	do {
		grid_t x = 0;
		do
			territory[y][x] = null_player;
		while (++x);
	} while (++y);
}

static void game_preload(struct game* const game) {
	// TODO: fix order
	game->x = 0;
	game->y = 0;
	grid_clear_all_uint8(game->map);
	grid_clear_all_uint8(game->territory);
	grid_clear_all_uint8(game->labels);
	grid_clear_all_energy_t(game->workspace);
	units_initialise(&game->units);
	game->selected = null_unit;
	queue_initialise(&game->queue);
	game->turn = 0;
	game_territory_initialise(game->territory);

	for (player_t player = 0; player < players_capacity; ++player) {
		game->golds[player] = 0;
		game->incomes[player] = 0;
	}
}

static void game_compute_incomes(struct game* const game) {
	grid_t y = 0;
	do {
		grid_t x = 0;
		do {
			const player_t player = game->territory[y][x];
			if (player != null_player)
				++game->incomes[player];
		} while (++x);
	} while (++y);
}

static void game_postload(struct game* const game) {
	game_compute_incomes(game);

	// Set players' alive state
	for (player_t player = 0; player < players_capacity; ++player)
		game->alives[player] = game->units.firsts[player] != null_unit;
}

bool game_load(struct game* const game, const char* const filename) {
	game_preload(game);
	const bool error = file_load(game, filename);
	game_postload(game);
	return error;
}

static bool game_parse_movement(struct game* const game, const char input) {
	switch (input) {
		case 'w': {
			game->prev_x = game->x;
			game->prev_y = game->y;
			--game->y;
			return true;
		}
		case 'a': {
			game->prev_x = game->x;
			game->prev_y = game->y;
			--game->x;
			return true;
		}
		case 's': {
			game->prev_x = game->x;
			game->prev_y = game->y;
			++game->y;
			return true;
		}
		case 'd': {
			game->prev_x = game->x;
			game->prev_y = game->y;
			++game->x;
			return true;
		}
	}
	return false;
}

static bool game_parse_file(struct game* const game, const char input) {
	bool error;

	switch (input) {
		case '1': {error = game_load(game, "state1.txt"); break;}
		case '2': {error = game_load(game, "state2.txt"); break;}
		case '3': {error = game_load(game, "state3.txt"); break;}
		case '4': {error = game_load(game, "state4.txt"); break;}
		case '5': {error = game_load(game, "state5.txt"); break;}
		case '6': {error = file_save(game, "state1.txt"); break;}
		case '7': {error = file_save(game, "state2.txt"); break;}
		case '8': {error = file_save(game, "state3.txt"); break;}
		case '9': {error = file_save(game, "state4.txt"); break;}
		case '0': {error = file_save(game, "state5.txt"); break;}
		default:
			return false;
	}

	if (error)
		printf("IO error");

	return true;
}

static bool game_attack_enabled(const struct game* const game) {
	// The state is attack enabled iff:
	// 1. A unit is selected
	// 2. Previous selected tile is accessible if direct attack
	// 3. Selected tile is attackable, which implies:
	//     a. Selected unit can attack with positive damage
	//     b. Attacker and attackee are in different teams
	return game->selected != null_unit &&
		(units_min_range[game->units.data[game->selected].model] || game->labels[game->prev_y][game->prev_x] & accessible_bit) &&
		game->labels[game->y][game->x] & attackable_bit;
}

static void game_handle_action(struct game* const game) {
	const unit_t unit = game->units.grid[game->y][game->x];

	if (game->selected == null_unit) {
		// Select unit
		if (unit != null_unit && game->units.data[unit].enabled) {
			game->selected = unit;
			grid_explore(game);
		}
	} else {
		// Cursor over selected unit
		if (game->selected == unit) {
			game->selected = null_unit;
			grid_clear_all_uint8(game->labels);
		// Move to accessible tile
		} else if (game->labels[game->y][game->x] & accessible_bit) {
			units_move(&game->units, game->selected, game->x, game->y);
			struct unit* const unit = &game->units.data[game->selected];
			// The moved unit can capture iff:
			// 1. The unit is a infantry or a mech
			// 2. The tile is capturable
			// 3. The tile is owned other than the player
			if (unit->model < unit_capturable_upper_bound &&
				game->map[game->y][game->x] >= tile_capturable_lower_bound &&
				game->territory[game->y][game->x] != unit->player)
				game->territory[game->y][game->x] = unit->player;
			unit->enabled = false;
			game->selected = null_unit;
			grid_clear_all_uint8(game->labels);
		}
	}
}

// Calculate damage when attacker attacks attackee
static health_wide_t calc_damage(
	const struct game* const game,
	const struct unit* const attacker,
	const struct unit* const attackee) {

	const tile_t tile = game->map[attackee->y][attackee->x];
	const uint8_t movement_type = unit_movement_types[attackee->model];
	return (
		(health_wide_t)units_damage[attacker->model][attackee->model] *
		(health_wide_t)(attacker->health + 1) *
		(health_wide_t)(10 - tile_defense[movement_type][tile])
	) / (100 * 10);
}

static void game_handle_attack(struct game* const game) {
	assert(game->units.grid[game->y][game->x] != null_unit);
	assert(game->selected != null_unit);

	struct unit* const attacker = &game->units.data[game->selected];
	const unit_t attackee_index = game->units.grid[game->y][game->x];
	struct unit* const attackee = &game->units.data[attackee_index];

	// Skip to post-attack cleanup when a unit dies
	do {
		// If unit is direct, move to attack
		const bool ranged = units_min_range[attacker->model];
		if (!ranged)
			units_move(&game->units, game->selected, game->prev_x, game->prev_y);

		const health_wide_t damage = calc_damage(game, attacker, attackee);

		// Apply damage
		if (damage > attackee->health) {
			units_delete(&game->units, attackee_index);
			break;
		}

		attackee->health -= damage;

		// Ranged units do not receive counter-attacks
		if (ranged)
			break;

		const health_wide_t counter_damage = calc_damage(game, attackee, attacker);

		// Apply counter damage
		if (counter_damage > attacker->health) {
			units_delete(&game->units, game->selected);
			break;
		}

		attacker->health -= counter_damage;
	} while (false);

	// Deselect attacker
	attacker->enabled = false;
	game->selected = null_unit;
	grid_clear_all_uint8(game->labels);
}

static void game_next_turn(struct game* const game) {
	units_set_enabled(&game->units, game->turn, false);

	const player_t prev_turn = game->turn;

	// Find next alive player
	do {
		game->turn = (game->turn + 1) % players_capacity;
	} while (!game->alives[game->turn] && game->turn != prev_turn);

	units_set_enabled(&game->units, game->turn, true);

	// Add income to golds
	game->golds[game->turn] += gold_scale * game->incomes[game->turn];
	printf(gold_format, game->golds[game->turn]);

	// Heal units on friendly capturables
	unit_t curr = game->units.firsts[game->turn];
	while (curr != null_unit) {
		struct unit* const unit = &game->units.data[curr];
		if (game->territory[unit->y][unit->x] == game->turn &&
			unit->health < health_max) {
			// Cap heal at maximum health
			const health_t heal_rate = health_max / gold_scale;
			if (unit->health >= health_max - heal_rate)
				unit->health = health_max;
			else
				unit->health += heal_rate;
			// Deduct heal cost
			game->golds[game->turn] -= units_cost[unit->model];
			printf(gold_format, game->golds[game->turn]);
		}

		curr = game->units.nexts[curr];
	}
}

void game_loop(struct game* const game) {
	render(game, false);

	char input = getch();

	while (input != 'q') {

		// Fix case skipping
		if (game_parse_movement(game, input) ||
			game_parse_file(game, input)) {}

		const bool attack_enabled = game_attack_enabled(game);

		if (input == ' ') {
			if (attack_enabled)
				game_handle_attack(game);
			else
				game_handle_action(game);
		} else if (input == 'n') {
			game_next_turn(game);
		}

		render(game, attack_enabled);

		if (game->territory[game->y][game->x] != null_player)
			printf("turn=%hhu x=%hhu y=%hhu tile=%s territory=%hhu attack=%u label=%u gold=%u", game->turn, game->x, game->y,
				tile_names[game->map[game->y][game->x]],
				game->territory[game->y][game->x],
				attack_enabled,
				game->labels[game->y][game->x],
				game->golds[game->turn]);
		else
			printf("turn=%hhu x=%hhu y=%hhu tile=%s territory=none attack=%u label=%u gold=%u", game->turn, game->x, game->y,
				tile_names[game->map[game->y][game->x]],
				attack_enabled,
				game->labels[game->y][game->x],
				game->golds[game->turn]);

		input = getch();
	}
}
