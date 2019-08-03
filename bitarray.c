#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "bitarray.h"

void bitarray_clear(uint8_t* const data, const player_wide_t size) {
	memset(data, 0, size);
}

void bitarray_set(uint8_t* const data, const player_wide_t index) {
	data[index >> 3] |= 1 << (index & 7);
}

void bitarray_unset(uint8_t* const data, const player_wide_t index) {
	data[index >> 3] &= ~(1 << (index & 7));
}

bool bitarray_get(const uint8_t* const data, const player_wide_t index) {
	return data[index >> 3] & (1 << (index & 7));
}

// Computes bitarray index from bitmatrix indices
static player_wide_t bitmatrix_cascade(const player_t p, const player_t q) {
	// Bit-matrix relation is reflexive and symmetric
	assert(p > q);

	return
		(player_wide_t)p +
		(player_wide_t)q * players_capacity -
		(((player_wide_t)q + 1) * ((player_wide_t)q + 2)) / 2;
}

void bitmatrix_set(uint8_t* const data, const player_t p, const player_t q) {
	if (p == null_player || q == null_player)
		return;

	if (p == q)
		return;

	if (p > q)
		bitarray_set(data, bitmatrix_cascade(p, q));
	else
		bitarray_set(data, bitmatrix_cascade(q, p));
}

void bitmatrix_unset(uint8_t* const data, const player_t p, const player_t q) {
	if (p == null_player || q == null_player)
		return;

	if (p == q)
		return;

	if (p > q)
		bitarray_unset(data, bitmatrix_cascade(p, q));
	else
		bitarray_unset(data, bitmatrix_cascade(q, p));
}

bool bitmatrix_get(const uint8_t* const data, const player_t p, const player_t q) {
	// Cannot be allied with null player
	if (p == null_player || q == null_player)
		return false;

	// Friendly untis are allied
	if (p == q)
		return true;

	// Alliance is symmetrical
	if (p > q)
		return bitarray_get(data, bitmatrix_cascade(p, q));
	else
		return bitarray_get(data, bitmatrix_cascade(q, p));
}

void bitmatrix_print(const uint8_t* const data) {
	for (player_t q = 0; q < players_capacity; ++q) {
		for (player_t p = 0; p < players_capacity; ++p)
			printf("%u ", bitmatrix_get(data, p, q));
		printf("\n");
	}
}
