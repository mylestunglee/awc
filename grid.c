#include <assert.h>
#include <stdlib.h>
#include "bitarray.h"
#include "definitions.h"
#include "grid.h"

void grid_clear_all_uint8(uint8_t grid[grid_size][grid_size]) {
	grid_t y = 0;
	do {
		grid_t x = 0;
		do {
			grid[y][x] = 0;
		} while (++x);
	} while (++y);
}

void grid_clear_all_energy(energy_t workspace[grid_size][grid_size]) {
	grid_t y = 0;
	do {
		grid_t x = 0;
		do
			workspace[y][x] = 0;
		while (++x);
	} while (++y);
}

void grid_clear_territory(player_t territory[grid_size][grid_size]) {
	grid_t y = 0;
	do {
		grid_t x = 0;
		do
			territory[y][x] = null_player;
		while (++x);
	} while (++y);
}

void grid_clear_player_territory(
	player_t territory[grid_size][grid_size],
	const player_t loser) {

	grid_t y = 0;
	do {
		grid_t x = 0;
		do
			if (territory[y][x] == loser)
				territory[y][x] = null_player;
		while (++x);
	} while (++y);
}

// Normalise invalid map territory state
void grid_correct_map(
	player_t territory[grid_size][grid_size],
	tile_t map[grid_size][grid_size]) {

	grid_t y = 0;
	do {
		grid_t x = 0;
		do {
			if (territory[y][x] > players_capacity) {
				territory[y][x] = null_player;
			}

			if (territory[y][x] == null_player && map[y][x] == tile_HQ)
				map[y][x] = tile_city;
		} while (++x);
	} while (++y);
}

void grid_compute_incomes(player_t territory[grid_size][grid_size], gold_t incomes[players_capacity]) {
	grid_t y = 0;
	do {
		grid_t x = 0;
		do {
			const player_t player = territory[y][x];
			if (player != null_player)
				++incomes[player];
		} while (++x);
	} while (++y);
}

// Marks a tile as attackable if position relates to attackable unit
static void grid_explore_mark_attackable_tile(
	struct game* const game,
	const grid_t x,
	const grid_t y,
	const model_t model,
	const player_t player,
	const bool label_attackable_tiles) {

	// By-pass checks when label attackable tiles forcefully
	if (label_attackable_tiles) {
		game->labels[y][x] |= attackable_bit;
		return;
	}

	const unit_t index = game->units.grid[y][x];
	const struct unit* const unit = &game->units.data[index];

	// Mark tiles without friendly units as attackable
	if (index != null_unit &&
		!bitmatrix_get(game->alliances, unit->player, player) &&
		units_damage[model][unit->model])
		game->labels[y][x] |= attackable_bit;
}

static void grid_explore_mark_attackable_direct(
	struct game* const game,
	const grid_t x,
	const grid_t y,
	const model_t model,
	const player_t player,
	const bool label_attackable_tiles) {

	// Direct attack only applies to 0-ranged models
	if(models_min_range[model])
		return;

	grid_explore_mark_attackable_tile(game, x + 1, y, model, player, label_attackable_tiles);
	grid_explore_mark_attackable_tile(game, x - 1, y, model, player, label_attackable_tiles);
	grid_explore_mark_attackable_tile(game, x, y + 1, model, player, label_attackable_tiles);
	grid_explore_mark_attackable_tile(game, x, y - 1, model, player, label_attackable_tiles);
}

static void grid_explore_mark_attackable_ranged(
	struct game* const game,
	const grid_t x,
	const grid_t y,
	const model_t model,
	const player_t player,
	const bool label_attackable_tiles) {

	// Range attack only applies to positive ranged models
	if(!models_min_range[model])
		return;

	for (grid_wide_t j = -models_max_range[model]; j <= models_max_range[model]; ++j)
		for (grid_wide_t i = -models_max_range[model]; i <= models_max_range[model]; ++i) {
			const grid_wide_t distance = abs(i) + abs(j);
			if (models_min_range[model] <= distance && distance <= models_max_range[model])
				grid_explore_mark_attackable_tile(game, x + i, y + j, model, player, label_attackable_tiles);
		}
}

// Recursively marks tiles that are accessible or attackable from the cursor tile
void grid_explore(const bool label_attackable_tiles, struct game* const game) {
	struct queue* const queue = &game->queue;

	assert(queue_empty(queue));
	assert(game->units.grid[game->y][game->x] != null_unit);

	const unit_t cursor_unit_index = game->units.grid[game->y][game->x];
	const struct unit* const cursor_unit = &game->units.data[cursor_unit_index];
	const uint8_t movement_type = unit_movement_types[cursor_unit->model];

	grid_explore_mark_attackable_ranged(game, game->x, game->y, cursor_unit->model, cursor_unit->player, label_attackable_tiles);

	queue_insert(queue, (struct queue_node){
		.x = game->x,
		.y = game->y,
		.energy =
			unit_movement_ranges[cursor_unit->model] +
			movement_type_cost[movement_type][game->map[game->y][game->x]]
		});

	while (!queue_empty(queue)) {
		const struct queue_node* const node = queue_remove(queue);

		const tile_t tile = game->map[node->y][node->x];
		const energy_t cost = movement_type_cost[movement_type][tile];

		// Inaccessible terrian
		if (cost == 0)
			continue;

		// Not enough energy to keep moving
		if (node->energy < cost)
			continue;

		// Cannot pass through enemy units
		const unit_t node_unit_index = game->units.grid[node->y][node->x];
		if (node_unit_index != null_unit &&
			!bitmatrix_get(
				game->alliances,
				game->units.data[node_unit_index].player,
				cursor_unit->player))
			continue;

		const energy_t energy = node->energy - cost;

		// Do not re-compute explored areas
		if (game->workspace[node->y][node->x] > energy)
			continue;
		else
			game->workspace[node->y][node->x] = node->energy;


		// Mark unit-free tiles as accessible but ships cannot block bridges
		if ((node_unit_index == null_unit || node_unit_index == cursor_unit_index) &&
			(tile != tile_bridge || movement_type != movement_type_ship)) {

			game->labels[node->y][node->x] |= accessible_bit;
			grid_explore_mark_attackable_direct(game, node->x, node->y, cursor_unit->model, cursor_unit->player, label_attackable_tiles);
		}

		// Explore adjacent tiles
		queue_insert(queue, (struct queue_node){.x = node->x + 1, .y = node->y, .energy = energy});
		queue_insert(queue, (struct queue_node){.x = node->x - 1, .y = node->y, .energy = energy});
		queue_insert(queue, (struct queue_node){.x = node->x, .y = node->y + 1, .energy = energy});
		queue_insert(queue, (struct queue_node){.x = node->x, .y = node->y - 1, .energy = energy});
	}

	// Allow stuck units to wait
	game->labels[game->y][game->x] |= accessible_bit;

	// Clean-up temporary memory
	grid_clear_all_energy(game->workspace);
}
