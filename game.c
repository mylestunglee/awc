#include <stdio.h>
#include "console.h"
#include "game.h"
#include "graphics.h"
#include "grid.h"
#include "file.h"
#include "bitarray.h"
#include "bot.h"
#include "action.h"

static void game_preload(struct game* const game) {
	// TODO: fix order
	game->x = 0;
	game->y = 0;
	grid_clear_uint8(game->map);
	grid_clear_uint8(game->labels);
	grid_clear_energy(game->energies);
	grid_clear_territory(game->territory);
	units_initialise(&game->units);
	game->selected = null_unit;
	list_initialise(&game->list);
	game->turn = 0;

	for (player_t player = 0; player < players_capacity; ++player) {
		game->golds[player] = 0;
		game->incomes[player] = 0;
	}

	bitarray_clear(game->bots, sizeof(game->bots));
	bitarray_clear(game->alliances, sizeof(game->alliances));
}

static void game_postload(struct game* const game) {
	grid_correct_map(game->territory, game->map);
	grid_compute_incomes(game->territory, game->incomes);
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

// Build try to build a unit, assume build enabled
static bool game_parse_build(struct game* const game, const char input) {
	const tile_t capturable = game->map[game->y][game->x] - terrian_capacity;
	const model_t value = input - '1';

	if (value >= buildable_models[capturable + 1] - buildable_models[capturable])
		return false;

	return !action_build(game, value + buildable_models[capturable]);
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

static bool game_build_enabled(const struct game* const game) {
	// The state is build enabled iff:
	// 1. The player owns the selected capturable
	// 2. There is no unit on the tile
	// 3. The capturable has buildable units
	// 4. No unit is selected

	if (game->territory[game->y][game->x] != game->turn)
		return false;

	const tile_t capturable = game->map[game->y][game->x] - terrian_capacity;

	return
		game->units.grid[game->y][game->x] == null_unit &&
		buildable_models[capturable] < buildable_models[capturable + 1] &&
		game->selected == null_unit;
}

static bool game_attack_enabled(const struct game* const game) {
	// The state is attack enabled iff:
	// 1. A unit is selected
	// 2. Previous selected tile is accessible if direct attack
	// 3. Selected tile is attackable, which implies:
	//     a. Selected unit can attack with positive damage
	//     b. Attacker and attackee are in different teams
	return game->selected != null_unit &&
		(models_min_range[game->units.data[game->selected].model] || game->labels[game->prev_y][game->prev_x] & accessible_bit) &&
		game->labels[game->y][game->x] & attackable_bit;
}

static void game_handle_action(struct game* const game) {
	const unit_t unit = game->units.grid[game->y][game->x];

	// Select unit iff:
	// 1. A unit is not already selected
	// 2. The unit is enabled
	// 3. The unit has possible moves
	if (game->selected == null_unit && unit != null_unit) {
		const bool select = game->units.data[unit].enabled;

		// Remove highlighting of disabled units
		if (select) {
			grid_clear_uint8(game->labels);
		}

		// Allow highlighting of disabled units
		grid_explore(game, !select);
		grid_clear_energy(game->energies);

		if (select)
			game->selected = unit;
	} else {
		// Move to accessible tile when cursor is not over unit
		if (game->selected != unit &&
			game->labels[game->y][game->x] & accessible_bit) {

			assert(game->selected != null_unit);

			units_move(&game->units, game->selected, game->x, game->y);
			action_handle_capture(game);
			game->units.data[game->selected].enabled = false;
		}

		game->selected = null_unit;
		grid_clear_uint8(game->labels);
	}
}

// Calculate damage when attacker attacks attackee
static health_t calc_damage(
	const struct game* const game,
	const struct unit* const attacker,
	const struct unit* const attackee) {

	const tile_t tile = game->map[attackee->y][attackee->x];
	const uint8_t movement_type = unit_movement_types[attackee->model];
	return (
		(health_wide_t)units_damage[attacker->model][attackee->model] *
		(health_wide_t)(attacker->health + 1) *
		(health_wide_t)(defense_max - tile_defense[movement_type][tile])
	) / (attack_max * defense_max);
}

// Calculate damage and counter-damage values without performing attack
void game_simulate_attack(
	const struct game* const game,
	health_t* const damage,
	health_t* const counter_damage) {

	assert(game->selected != null_unit);

	const struct unit* const attacker = &game->units.data[game->selected];
	const unit_t attackee_index = game->units.grid[game->y][game->x];
	const struct unit* const attackee = &game->units.data[attackee_index];

	*damage = calc_damage(game, attacker, attackee);

	// Apply damage
	if (*damage > attackee->health) {
		*counter_damage = 0;
		return;
	}

	// Ranged units do not receive to give counter-attacks
	if (models_min_range[attacker->model] || models_min_range[attackee->model])
		*counter_damage = 0;
	else
		*counter_damage = calc_damage(game, attackee, attacker);
}

static void game_attack(struct game* const game) {
	action_attack(game);

        game->units.data[game->selected].enabled = false;
	game->selected = null_unit;
	grid_clear_uint8(game->labels);
}

static void repair_units(struct game* const game)
{
	unit_t curr = game->units.firsts[game->turn];
	while (curr != null_unit) {
		struct unit* const unit = &game->units.data[curr];
		if (game->territory[unit->y][unit->x] == game->turn &&
			unit->health < health_max) {
			// Cap heal at maximum health
			if (unit->health >= health_max - heal_rate)
				unit->health = health_max;
			else
				unit->health += heal_rate;
			// Deduct heal cost
			game->golds[game->turn] -= models_cost[unit->model];
		}

		curr = game->units.nexts[curr];
	}
}

// A player is alive iff:
// 1. The player has units
// 2. The player has a HQ, implied by a positive income
//    This holds because when a player loses their HQ, income is nullified
static bool is_alive(const struct game* const game, const player_t player) {
	return game->units.firsts[player] != null_unit || game->incomes[player] > 0;
}

static void end_turn(struct game* const game) {
	game->selected = null_unit;
	grid_clear_uint8(game->labels);
	units_set_enabled(&game->units, game->turn, false);
}

static void start_turn(struct game* const game) {
	units_set_enabled(&game->units, game->turn, true);
	game->golds[game->turn] += gold_scale * game->incomes[game->turn];
	repair_units(game);
}

static bool is_bot(const struct game* const game, const player_t player) {
	return bitarray_get(game->bots, player);
}

static void next_alive_turn(struct game* const game) {
	for (player_t i = 0; i < players_capacity; ++i) {
		game->turn = (game->turn + 1) % players_capacity;
		if (is_alive(game, game->turn))
			return;
	}
	assert (false);
}

static bool exists_alive_non_bot(const struct game* const game) {
	for (player_t player = 0; player < players_capacity; ++player)
		if (is_alive(game, player) && !is_bot(game, player))
			return true;

	return false;
}

static void next_turn(struct game* const game) {
	do {
		if (is_bot(game, game->turn))
			bot_play(game);

		end_turn(game);
		next_alive_turn(game);
		start_turn(game);
	} while (exists_alive_non_bot(game) && is_bot(game, game->turn));
}

static void print_normal_text(const struct game* const game) {
	printf("turn=%hhu x=%hhu y=%hhu tile=%s territory=%hhu label=%u gold=%u", game->turn, game->x, game->y,
		tile_names[game->map[game->y][game->x]],
		game->territory[game->y][game->x],
		game->labels[game->y][game->x],
		game->golds[game->turn]);
}

static void print_attack_text(const struct game* const game) {
	health_t damage, counter_damage;
	game_simulate_attack(game, &damage, &counter_damage);
	const health_wide_t percent = 100;
	printf("Damage: %u%% Counter-damage: %u%%",
		(damage * percent) / health_max,
		(counter_damage * percent) / health_max);
}

static void print_build_text(const struct game* const game) {
	const tile_t tile = game->map[game->y][game->x];
	assert(tile >= terrian_capacity);
	const tile_t capturable = tile - terrian_capacity;

	printf("in build mode:");
	for (model_t model = buildable_models[capturable]; model < buildable_models[capturable + 1]; ++model) {
		printf("("model_format") %s ", model + 1, model_names[model]);
	}
}

static void print_text(
	const struct game* const game,
	const bool attack_enabled,
	const bool build_enabled) {

	if (attack_enabled)
		print_attack_text(game);
	else if (build_enabled)
		print_build_text(game);
	else
		print_normal_text(game);
}

void game_loop(struct game* const game) {
	do {
		bool attack_enabled = game_attack_enabled(game);
		bool build_enabled = game_build_enabled(game);
		assert(!(attack_enabled && build_enabled));

		render(game, attack_enabled, build_enabled);
		print_text(game, attack_enabled, build_enabled);

		char input = getch();

		if (input == '\n' || input == 'q')
			break;

		// Fix case skipping
		game_parse_movement(game, input);

		// Compute possible actions
		attack_enabled = game_attack_enabled(game);
		build_enabled = game_build_enabled(game);


		// Switch 0-9 keys between building and save/loading states
		if (build_enabled) {
			// Assume tile to be capturable
			assert(game->map[game->y][game->x] >= terrian_capacity);

			if (game_parse_build(game, input))
				build_enabled = false;
		} else
			game_parse_file(game, input);

		if (input == ' ') {
			if (attack_enabled) {
				game_attack(game);
			} else
				game_handle_action(game);
		} else if (input == 'n') {
			next_turn(game);
		}
	} while (true);

	// New line after print text
	printf("\n");
}
