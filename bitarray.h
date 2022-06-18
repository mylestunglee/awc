#ifndef BITARRAY_H
#define BITARRAY_H

#include "definitions.h"
#include <stdbool.h>

typedef uint16_t player_wide_t;

#define BITARRAY_SIZE(bits) (bits + 7) / 8
#define BITMATRIX_SIZE(bits) BITARRAY_SIZE((bits * (bits - 1)) / 2)

void bitarray_unset(uint8_t* const, const player_wide_t);
void bitarray_set(uint8_t* const, const player_wide_t);
bool bitarray_get(const uint8_t* const, const player_wide_t);
#ifdef EXPOSE_UNITS_INTERNALS
player_wide_t bitmatrix_cascade(const player_t, const player_t);
#endif
void bitmatrix_set(uint8_t* const, const player_t, const player_t);
void bitmatrix_unset(uint8_t* const, const player_t, const player_t);
bool bitmatrix_get(const uint8_t* const, const player_t, const player_t);

#endif
