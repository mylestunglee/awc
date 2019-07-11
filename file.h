#ifndef file_h
#define file_h

#include <stdbool.h>
#include "game.h"

bool file_load(const struct game* const, const uint8_t* const);
bool file_save(const struct game* const, const uint8_t* const);

#endif
