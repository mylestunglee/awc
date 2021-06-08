#ifndef parse_h
#define parse_h

#include "game.h"

bool parse_file(struct game* const game, const char input);
bool parse_panning(struct game* const game, const char input);
bool parse_surrender(struct game* const game, const char input);
bool parse_select_next_unit(struct game* const game, const char input);
bool parse_build(struct game* const game, const char input);
bool parse_self_destruct_unit(struct game* const game, const char input);
bool parse_space(struct game* const game, const char input,
                 const bool attack_enabled);
bool parse_command(struct game* const game, const char input,
                   bool attack_enabled, bool build_enabled);

#endif
