#include <string.h>
#include "bitarray.h"

void bitarray_clear(uint8_t* const data, const player_wide_t size) {
	memset(data, 0, size);
}

void bitarray_set(uint8_t* const data, const player_wide_t index) {
	data[index >> 3] |= 1 << (index & 7);
}

bool bitarray_get(const uint8_t* const data, const player_wide_t index) {
	return data[index >> 3] & (1 << (index & 3));
}
