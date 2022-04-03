#ifndef BOT_H
#define BOT_H
#include "game.h"

#ifdef EXPOSE_BOT_INTERNALS
const struct unit* find_attackee(struct game* const,
                                 const struct unit* const);
void prepare_ranged_attack(struct game* const,
                          const struct unit* const);
void prepare_direct_attack(struct game* const,
                                 struct unit* const,
                                 const struct unit* const);
#endif
void bot_play(struct game* const game);

#endif
