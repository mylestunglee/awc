#ifndef path_h
#define path_h

#include "definitions.h"

struct path {
	grid_index size;
	uint8_t data[path_byte_capacity];
};

void path_initialise(struct path* const);
uint8_t path_top(const struct path* const);
void path_push(struct path* const);
uint8_t path_next(struct path* const);
void path_print(struct path* const);

#endif
