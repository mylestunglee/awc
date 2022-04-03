#ifndef BOT_H
#define BOT_H
#include "game.h"

#ifdef EXPOSE_BOT_INTERNALS
const struct unit* find_attackee(struct game* const, const model_t);
void set_prev_position(struct game* const, const model_t,
                       const struct unit* const);
void prepare_attack(struct game* const, const model_t,
                    const struct unit* const);
#endif
void bot_play(struct game* const game);

#endif
