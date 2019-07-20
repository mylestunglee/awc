#include <assert.h>
#include <stdlib.h>
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

void grid_clear_all_energy_t(uint16_t grid[grid_size][grid_size]) {
    grid_t y = 0;
    do {
        grid_t x = 0;
        do {
            grid[y][x] = 0;
        } while (++x);
    } while (++y);
}

// Marks a tile as attackable if position relates to attackable unit
static void grid_explore_mark_attackable_tile(
	struct game* const game,
	const grid_t x,
	const grid_t y,
	const model_t model,
	const player_t player) {

	const unit_t index = game->units.grid[y][x];
	const struct unit* const unit = &game->units.data[index];

	// Mark tiles without friendly units as attackable
	if (index != null_unit &&
		unit->player != player &&
		units_damage[model][unit->model])
		game->labels[y][x] |= attackable_bit;
}

static void grid_explore_mark_attackable_direct(
	struct game* const game,
	const grid_t x,
	const grid_t y,
	const model_t model,
	const player_t player) {

	// Direct attack only applies to 0-ranged models
	if(units_min_range[model])
		return;

	grid_explore_mark_attackable_tile(game, x + 1, y, model, player);
	grid_explore_mark_attackable_tile(game, x - 1, y, model, player);
	grid_explore_mark_attackable_tile(game, x, y + 1, model, player);
	grid_explore_mark_attackable_tile(game, x, y - 1, model, player);
}

static void grid_explore_mark_attackable_ranged(
	struct game* const game,
	const grid_t x,
	const grid_t y,
	const model_t model,
	const player_t player) {

	// Range attack only applies to positive ranged models
	if(!units_min_range[model])
		return;

	for (grid_wide_t j = -units_max_range[model]; j <= units_max_range[model]; ++j)
		for (grid_wide_t i = -units_max_range[model]; i <= units_max_range[model]; ++i) {
			const grid_wide_t distance = abs(i) + abs(j);
			if (units_min_range[model] <= distance && distance <= units_max_range[model])
				grid_explore_mark_attackable_tile(game, x + i, y + j, model, player);
		}
}


void grid_explore(struct game* const game) {
	struct queue* const queue = &game->queue;

	assert(queue_empty(queue));
	assert(game->units.grid[game->y][game->x] != null_unit);

	const model_t model = game->units.data[game->selected].model;
	const uint8_t movement_type = unit_movement_types[model];
	const player_t player = game->units.data[game->selected].player;

	grid_explore_mark_attackable_ranged(game, game->x, game->y, model, player);

	queue_insert(queue, (struct queue_node){
		.x = game->x,
		.y = game->y,
		.energy =
			unit_movement_ranges[model] +
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
		const unit_t unit = game->units.grid[node->y][node->x];
		if (unit != null_unit && game->units.data[unit].player != player)
			continue;

		const energy_t energy = node->energy - cost;

		// Do not re-compute explored areas
		if (game->workspace[node->y][node->x] > energy)
			continue;
		else
			game->workspace[node->y][node->x] = node->energy;


		// Mark unit-free tiles as accessible but ships cannot block bridges
		if ((unit == null_unit || unit == game->selected) &&
			(tile != tile_bridge || movement_type != movement_type_ship)) {

			game->labels[node->y][node->x] |= accessible_bit;

			grid_explore_mark_attackable_direct(game, node->x, node->y, model, player);
		}

		// Explore adjacent tiles
		queue_insert(queue, (struct queue_node){.x = node->x + 1, .y = node->y, .energy = energy});
		queue_insert(queue, (struct queue_node){.x = node->x - 1, .y = node->y, .energy = energy});
		queue_insert(queue, (struct queue_node){.x = node->x, .y = node->y + 1, .energy = energy});
		queue_insert(queue, (struct queue_node){.x = node->x, .y = node->y - 1, .energy = energy});
	}

	grid_clear_all_energy_t(game->workspace);
}

