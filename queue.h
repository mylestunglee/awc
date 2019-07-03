#ifndef queue_h
#define queue_h

#include <stdbool.h>
#include "definitions.h"

struct queue_node {
	grid_index x;
	grid_index y;
	unit_energy energy;
};

struct queue {
	queue_index start;
	queue_index end;
	struct queue_node nodes[queue_capacity];
};

void queue_initialise(struct queue* const);

bool queue_empty(const struct queue* const);

void queue_insert(struct queue* const, const struct queue_node);

struct queue_node* queue_remove(struct queue* const);

#endif
