#ifndef action_h
#define action_h

#include "game.h"

void action_remove_player(struct game* const, const player_t);
void action_handle_capture(struct game* const);
void action_attack(struct game* const);
bool action_build(struct game* const, const model_t);

#endif
