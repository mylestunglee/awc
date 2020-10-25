#include <assert.h>
#include "queue.h"

void queue_initialise(struct queue* const queue) {
	queue->start = 0;
	queue->end = 0;
}

bool queue_empty(const struct queue* const queue) {
	return queue->start == queue->end;
}

void queue_insert(struct queue* const queue, const struct queue_node node) {
	// If full then silently fail
	if (queue->start == (queue_t)(queue->end + 1))
		return;

	queue->nodes[queue->end] = node;
	++queue->end;
}

struct queue_node* queue_pop(struct queue* const queue) {
	// Assume queue is non-empty
	assert(!queue_empty(queue));

	struct queue_node* const node = &queue->nodes[queue->start];
	++queue->start;
	return node;
}
