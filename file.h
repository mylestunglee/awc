#ifndef FILE_H
#define FILE_H

#include "game.h"
#include <stdbool.h>

bool file_load(struct game* const, const char* const);
bool file_save(const struct game* const, const char* const);

#endif
