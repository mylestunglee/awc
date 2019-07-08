#include <stdio.h>
#include "console.h"
#include "game.h"
#include "graphics.h"
#include "grid.h"

static void game_map_initialise(tile_t map[grid_size][grid_size]) {
	grid_t y = 0;
	do {
		grid_t x = 0;
		do {
			map[y][x] = 0;
		} while (++x);
	} while (++y);

	for (int i = 0; i < 20; i++) {
		for (int j = 0; j < 6; j++) {
			if (i < 9) {
				map[j][i] = 1;
			} else {
				map[j][i] = 5;
			}
		}
	}

	for (int i = 0; i < tile_capacity; i++) {
		map[4][i] = i;
	}

	map[4][10] = 1;
	map[5][10] = 1;
}

void game_initialise(struct game* const game) {
	// TODO: fix order
	game->x = 0;
	game->y = 0;
	game_map_initialise(game->map);
	grid_clear_all_uint8(game->labels);
	grid_clear_all_energy_t(game->workspace);
	units_initialise(&game->units);
	game->selected = null_unit;
	queue_initialise(&game->queue);
}

static void game_parse_movement(struct game* const game, const uint8_t input) {
	switch (input) {
		case 'w': {
			game->prev_x = game->x;
			game->prev_y = game->y;
			--game->y;
			break;
		}
		case 'a': {
			game->prev_x = game->x;
			game->prev_y = game->y;
			--game->x;
			break;
		}
		case 's': {
			game->prev_x = game->x;
			game->prev_y = game->y;
			++game->y;
			break;
		}
		case 'd': {
			game->prev_x = game->x;
			game->prev_y = game->y;
			++game->x;
			break;
		}
	}
}

static bool game_attack_actionable(const struct game* const game) {
	// 1. A unit is selected
	// 2. Previous selected tile is accessible
	// 3. Selected tile is attackable, which implies:
	//     a. Selected unit can attack with positive damage
	//     b. Attacker and attackee are in different teams
	return game->selected != null_unit &&
		(game->labels[game->prev_y][game->prev_x] & accessible_bit) != 0 &&
		(game->labels[game->y][game->x] & attackable_bit) != 0;
}

static void game_handle_action(struct game* const game) {
	const unit_t unit = game->units.grid[game->y][game->x];
	if (game->selected == null_unit) {
		// Select unit
		if (unit != null_unit) {
			game->selected = unit;
			grid_explore(game);
		}
	} else {
		// Cursor over selected unit
		if (game->selected == unit) {
			game->selected = null_unit;
			grid_clear_all_uint8(game->labels);
		// Move to accessible tile
		} else if ((game->labels[game->y][game->x] & accessible_bit) != 0) {
			units_move(&game->units, game->selected, game->x, game->y);
			game->selected = null_unit;
			grid_clear_all_uint8(game->labels);
		}
	}
}

// Calculate damage when attacker attacks attackee
static health_t calc_damage(
	const struct game* const game,
	const struct unit* const attacker,
	const struct unit* const attackee) {

	const tile_t tile = game->map[attackee->y][attackee->x];
	const uint8_t movement_type = unit_movement_types[attackee->model];

	return (health_t)((
		(health_t_wide)units_damage[attacker->model][attackee->model] *
		(health_t_wide)(attacker->health + 1) *
		(health_t_wide)(10 - grid_defense[movement_type][tile])
	) / (100 * 10));
}

static void game_handle_attack(struct game* const game) {
	assert(game->units.grid[game->y][game->x] != null_unit);
	assert(game->selected != null_unit);

	struct unit* const attacker = &game->units.data[game->selected];
	const unit_t attackee_index = game->units.grid[game->y][game->x];
	struct unit* const attackee = &game->units.data[attackee_index];

	// Skip to post-attack cleanup when a unit dies
	do {
		// Prepare attacker for counter-attack
		units_move(&game->units, game->selected, game->prev_x, game->prev_y);

		const health_t damage = calc_damage(game, attacker, attackee);

		// Apply damage
		if (damage > attackee->health) {
			units_delete(&game->units, attackee_index);
			break;
		}

		attackee->health -= damage;

		const health_t counter_damage = calc_damage(game, attackee, attacker);

		// Apply counter damage
		if (counter_damage > attacker->health) {
			units_delete(&game->units, game->selected);
			break;
		}

		attacker->health -= counter_damage;
	} while (false);

	// Deselect attacker
	game->selected = null_unit;
	grid_clear_all_uint8(game->labels);
}

void game_loop(struct game* const game) {
	bool attack_actionable = game_attack_actionable(game);

	render(game, attack_actionable);

	char input = getch();

	while (input != 'q') {
		game_parse_movement(game, input);

		attack_actionable = game_attack_actionable(game);

		if (input == ' ') {
			if (attack_actionable)
				game_handle_attack(game);
			else
				game_handle_action(game);
		}

		render(game, attack_actionable);

		printf("%u %u", game->x, game->y);

		input = getch();
	}
}
