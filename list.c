#include "list.h"
#include <assert.h>

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

    list->nodes[list->end++] = node;
}

struct list_node list_front_pop(struct list* const list) {
    assert(!list_empty(list));

    return list->nodes[list->start++];
}

struct list_node list_back_pop(struct list* const list) {
    assert(!list_empty(list));

    return list->nodes[--list->end];
}

struct list_node list_front_peek(const struct list* const list) {
    assert(!list_empty(list));

    return list->nodes[list->start];
}

struct list_node list_back_peek(const struct list* const list) {
    assert(!list_empty(list));

    return list->nodes[list->end - 1];
}
