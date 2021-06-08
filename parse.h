#ifndef parse_h
#define parse_h

#include "game.h"

bool parse_file(struct game* const game, const char input);
bool parse_panning(struct game* const game, const char input);
bool parse_surrender(struct game* const game, const char input);
bool parse_select_next_unit(struct game* const game, const char input);

#endif
