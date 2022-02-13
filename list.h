#ifndef LIST_H
#define LIST_H

#include "definitions.h"
#include <stdbool.h>

typedef uint16_t energy_t;

struct list_node {
    grid_t x;
    grid_t y;
    energy_t energy;
};

typedef uint16_t list_t;
#define LIST_CAPACITY 0x10000

struct list {
    list_t start;
    list_t end;
    struct list_node nodes[LIST_CAPACITY];
};

void list_initialise(struct list* const);
bool list_empty(const struct list* const);
void list_insert(struct list* const, const struct list_node* const);
struct list_node list_front_pop(struct list* const);
struct list_node list_back_pop(struct list* const);
struct list_node list_front_peek(const struct list* const);
struct list_node list_back_peek(const struct list* const);

#endif
