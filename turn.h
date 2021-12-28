#ifndef turn_h
#define turn_h

#include "game.h"

#ifdef expose_turn_internals
void repair_units(struct game* const);
#endif
void turn_next(struct game* const game);

#endif
