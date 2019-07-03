#include <assert.h>
#include "grid.h"

void grid_clear_all_8(uint8_t grid[grid_size][grid_size]) {
    grid_index y = 0;
    do {
        grid_index x = 0;
        do {
            grid[y][x] = 0;
        } while (++x);
    } while (++y);
}

void grid_clear_all_16(uint16_t grid[grid_size][grid_size]) {
    grid_index y = 0;
    do {
        grid_index x = 0;
        do {
            grid[y][x] = 0;
        } while (++x);
    } while (++y);
}

void grid_explore(struct game* const game) {
	struct queue* const queue = &game->queue;

	assert(queue_empty(queue));

	queue_insert(queue, (struct queue_node){.x = game->x, .y = game->y, .energy = 4});

	while (!queue_empty(queue)) {
		const struct queue_node* const node = queue_remove(queue);

		const unit_energy cost = 1;

		if (node->energy < cost)
			continue;

		const unit_energy energy = node->energy - cost;

		if (game->workspace[node->y][node->x] > energy)
			continue;

		game->labels[node->y][node->x] |= accessible_bit;
		game->labels[node->y][(grid_index)(node->x + 1)] |= attackable_bit;
		game->labels[node->y][(grid_index)(node->x - 1)] |= attackable_bit;
		game->labels[(grid_index)(node->y + 1)][node->x] |= attackable_bit;
		game->labels[(grid_index)(node->y - 1)][node->x] |= attackable_bit;

		queue_insert(queue, (struct queue_node){.x = node->x + 1, .y = node->y, .energy = energy});
		queue_insert(queue, (struct queue_node){.x = node->x - 1, .y = node->y, .energy = energy});
		queue_insert(queue, (struct queue_node){.x = node->x, .y = node->y + 1, .energy = energy});
		queue_insert(queue, (struct queue_node){.x = node->x, .y = node->y - 1, .energy = energy});
	}
}

