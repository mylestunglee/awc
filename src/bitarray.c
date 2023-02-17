#include "bitarray.h"
#include "constants.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

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
player_wide_t bitmatrix_cascade(const player_t p, const player_t q) {
    // Bit-matrix relation is reflexive and symmetric
    assert(p < PLAYERS_CAPACITY);
    assert(p > q);
    const player_wide_t wide_q = q;
    return (player_wide_t)p + wide_q * PLAYERS_CAPACITY -
           ((wide_q + 1) * (wide_q + 2)) / 2;
}

void bitmatrix_set(uint8_t* const data, const player_t p, const player_t q) {
    if (p == NULL_PLAYER || q == NULL_PLAYER)
        return;

    if (p == q)
        return;

    if (p > q)
        bitarray_set(data, bitmatrix_cascade(p, q));
    else
        bitarray_set(data, bitmatrix_cascade(q, p));
}

void bitmatrix_unset(uint8_t* const data, const player_t p, const player_t q) {
    if (p == NULL_PLAYER || q == NULL_PLAYER)
        return;

    if (p == q)
        return;

    if (p > q)
        bitarray_unset(data, bitmatrix_cascade(p, q));
    else
        bitarray_unset(data, bitmatrix_cascade(q, p));
}

bool bitmatrix_get(const uint8_t* const data, const player_t p,
                   const player_t q) {
    // Cannot be allied with null player
    if (p == NULL_PLAYER || q == NULL_PLAYER)
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
