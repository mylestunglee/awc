#ifndef action_h
#define action_h

#include "game.h"

void action_attack(struct game* const);
bool action_build(struct game* const, const model_t);
#ifdef expose_action_internals
bool can_selected_unit_capture(const struct game* const);
#endif
bool action_move(struct game* const game);
bool action_self_destruct_selection(struct game* const game);
bool action_surrender(struct game* const game);
bool action_select(struct game* const game);
bool action_highlight(struct game* const game);

#ifdef expose_action_internals

bool at_least_two_alive_players(const struct game* const game)

#endif

#endif
