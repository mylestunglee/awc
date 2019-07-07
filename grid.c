#include <assert.h>
#include "grid.h"

void grid_clear_all_uint8(uint8_t grid[grid_size][grid_size]) {
    grid_index y = 0;
    do {
        grid_index x = 0;
        do {
            grid[y][x] = 0;
        } while (++x);
    } while (++y);
}

void grid_clear_all_unit_energy(uint16_t grid[grid_size][grid_size]) {
    grid_index y = 0;
    do {
        grid_index x = 0;
        do {
            grid[y][x] = 0;
        } while (++x);
    } while (++y);
}

static void grid_explore_mark_attackable(
	struct game* const game,
	const grid_index x,
	const grid_index y,
	const player_index player) {

	const unit_index unit = game->units.grid[y][x];

	// Mark tiles without friendly units as attackable
	if (unit == null_unit || game->units.data[unit].player != player)
		game->labels[y][x] |= attackable_bit;
}

void grid_explore(struct game* const game) {
	struct queue* const queue = &game->queue;

	assert(queue_empty(queue));
	assert(game->units.grid[game->y][game->x] != null_unit);

	const unit_model model = game->units.data[game->units.grid[game->y][game->x]].model;
	const uint8_t movement_type = unit_movement_types[model];
	const player_index player = game->units.data[game->units.grid[game->y][game->x]].player;

	queue_insert(queue, (struct queue_node){
		.x = game->x,
		.y = game->y,
		.energy =
			unit_movement_ranges[model] +
			movement_type_cost[movement_type][game->map[game->y][game->x]]
		});

	while (!queue_empty(queue)) {
		const struct queue_node* const node = queue_remove(queue);

		const tile_index tile = game->map[node->y][node->x];
		const unit_energy cost = movement_type_cost[movement_type][tile];

		// Inaccessible terrian
		if (cost == 0)
			continue;

		// Not enough energy to keep moving
		if (node->energy < cost)
			continue;

		const unit_energy energy = node->energy - cost;

		// Do not re-compute explored areas
		if (game->workspace[node->y][node->x] > energy)
			continue;
		else
			game->workspace[node->y][node->x] = node->energy;

		// Mark unit-free tiles as accessible but ships cannot block bridges
		if (game->units.grid[node->y][node->x] == null_unit &&
			(tile != tile_bridge || movement_type != movement_type_ship)) {

			game->labels[node->y][node->x] |= accessible_bit;
			grid_explore_mark_attackable(game, node->x + 1, node->y, player);
			grid_explore_mark_attackable(game, node->x - 1, node->y, player);
			grid_explore_mark_attackable(game, node->x, node->y + 1, player);
			grid_explore_mark_attackable(game, node->x, node->y - 1, player);
		}

		queue_insert(queue, (struct queue_node){.x = node->x + 1, .y = node->y, .energy = energy});
		queue_insert(queue, (struct queue_node){.x = node->x - 1, .y = node->y, .energy = energy});
		queue_insert(queue, (struct queue_node){.x = node->x, .y = node->y + 1, .energy = energy});
		queue_insert(queue, (struct queue_node){.x = node->x, .y = node->y - 1, .energy = energy});
	}
}

