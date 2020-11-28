#include <assert.h>
#include "list.h"

void list_initialise(struct list* const list) {
	list->start = 0;
	list->end = 0;
}

bool list_empty(const struct list* const list) {
	return list->start == list->end;
}

void list_insert(struct list* const list, const struct list_node node) {
	// If full then silently fail
	if (list->start == (list_t)(list->end + 1))
		return;

	list->nodes[list->end] = node;
	++list->end;
}

struct list_node* list_pop(struct list* const list) {
	// Assume list is non-empty
	assert(!list_empty(list));

	struct list_node* const node = &list->nodes[list->start];
	++list->start;
	return node;
}
