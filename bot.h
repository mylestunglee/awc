#ifndef BOT_H
#define BOT_H
#include "game.h"

#ifdef EXPOSE_BOT_INTERNALS
const struct unit* find_attackee(struct game* const game,
                                 const struct unit* const attacker);
#endif
void bot_play(struct game* const game);

#endif
