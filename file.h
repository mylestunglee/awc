#ifndef FILE_H
#define FILE_H

#include "game.h"
#include <stdbool.h>

#ifdef EXPOSE_FILE_INTERNALS
bool load_turn(const char* const, const char* const, player_t* const);
#endif
bool file_load(struct game* const, const char* const);
bool file_save(const struct game* const, const char* const);

#endif
