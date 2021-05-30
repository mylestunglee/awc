#ifndef bitarray_h
#define bitarray_h

#include "definitions.h"
#include <stdbool.h>

void bitarray_clear(uint8_t* const, const player_wide_t);
void bitarray_set(uint8_t* const, const player_wide_t);
void bitarray_unset(uint8_t* const, const player_wide_t);
bool bitarray_get(const uint8_t* const, const player_wide_t);
void bitmatrix_set(uint8_t* const, const player_t, const player_t);
void bitmatrix_unset(uint8_t* const, const player_t, const player_t);
bool bitmatrix_get(const uint8_t* const, const player_t, const player_t);
void bitmatrix_print(const uint8_t* const);

#endif
