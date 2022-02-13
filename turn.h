#ifndef TURN_H
#define TURN_H

#include "game.h"
#define HEAL_RATE (health_t)51

#ifdef expose_turn_internals
void repair_units(struct game* const);
void start_turn(struct game* const);
void end_turn(struct game* const);
void next_alive_turn(struct game* const);
bool exists_alive_non_bot(const struct game* const game);
#endif
void turn_next(struct game* const game);

#endif
