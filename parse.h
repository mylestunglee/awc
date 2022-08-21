#ifndef PARSE_H
#define PARSE_H

#include "game.h"

bool parse_quit(const char);
bool parse_file(struct game* const, const char);
bool parse_next_turn(struct game* const, const char);
bool parse_panning(struct game* const, const char);
bool parse_select_next_unit(struct game* const, const char);
bool parse_self_destruct(struct game* const, const char);
bool parse_surrender(struct game* const, const char);
bool parse_build(struct game* const, const char, const bool);
bool parse_attack(struct game* const, const bool);
bool parse_deselect(struct game* const);
bool parse_space(struct game* const, const char, const bool);
bool parse_command(struct game* const, const char, const bool attack_enabled,
                   const bool build_enabled);

#endif
