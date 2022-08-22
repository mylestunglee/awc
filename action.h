#ifndef ACTION_H
#define ACTION_H

#include "game.h"

#ifdef EXPOSE_ACTION_INTERNALS
health_t move_selected_unit(struct game* const, const grid_t, const grid_t);
#endif
bool action_attack(struct game* const);
bool action_build(struct game* const, const model_t);
#ifdef EXPOSE_ACTION_INTERNALS
bool can_selected_unit_capture(const struct game* const);
bool action_capture(struct game* const game);
#endif
bool action_move(struct game* const game);
bool action_self_destruct(struct game* const game);
#ifdef EXPOSE_ACTION_INTERNALS
bool at_least_two_alive_players(const struct game* const game);
#endif
bool action_surrender(struct game* const game);
bool action_select(struct game* const game);
bool action_highlight(struct game* const game);

#endif
