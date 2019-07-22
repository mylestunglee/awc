#ifndef bitarray_h
#define bitarray_h

#include <stdbool.h>
#include "definitions.h"

void bitarray_clear(uint8_t* const, const player_wide_t);
void bitarray_set(uint8_t* const, const player_wide_t);
bool bitarray_get(const uint8_t* const, const player_wide_t);

#endif
