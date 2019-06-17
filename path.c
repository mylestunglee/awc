#include <assert.h>
#include <stdio.h>
#include "path.h"

static uint8_t path_get(const uint8_t data[path_byte_capacity], const grid_index index) {
	assert(index < path_byte_capacity * 4);
	return (data[index >> 2] >> ((index & 3) << 1)) & 3;
}

static void path_set(uint8_t data[path_byte_capacity], const grid_index index) {
	assert(index < path_byte_capacity * 4);
	data[index >> 2] |= 3 << ((index & 3) << 1);
}

static void path_decrement(uint8_t data[path_byte_capacity], const grid_index index) {
	assert(index < 4 * path_byte_capacity);
	data[index >> 2] -= 1 << ((index & 3) << 1);
}

void path_initialise(struct path* const path) {
	path->size = 0;
}

uint8_t path_top(const struct path* const path) {
	assert(path->size > 0);
	return path_get(path->data, path->size - 1);
}

void path_push(struct path* const path) {
	assert(path->size < 4 * path_byte_capacity);
	path_set(path->data, path->size);
	++path->size;
}

uint8_t path_next(struct path* const path) {
	assert(path->size > 0);
	uint8_t backsteps = 0;
	while (path->size > 0 && path_top(path) == 0) {
		--path->size;
		++backsteps;
	}
	if (path->size > 0)
		path_decrement(path->data, path->size - 1);
	return backsteps;
}

void path_print(struct path* const path) {
	for (grid_index index = 0; index < path->size; ++index) {
		printf(grid_index_format, path_get(path->data, index));
	}
	printf("\n");
}
