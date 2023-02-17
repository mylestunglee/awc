#ifndef PARSE_H
#define PARSE_H

#include "units.h"

struct game;

#ifdef EXPOSE_PARSE_INTERNALS
bool parse_quit(struct game* const);
bool parse_nothing(struct game* const);
bool parse_build(struct game* const, const model_t offset);
bool parse_build_1(struct game* const);
bool parse_build_2(struct game* const);
bool parse_build_3(struct game* const);
bool parse_build_4(struct game* const);
bool parse_build_5(struct game* const);
bool parse_build_6(struct game* const);
bool parse_build_7(struct game* const);
bool parse_build_8(struct game* const);
bool parse_build_9(struct game* const);
bool parse_build_10(struct game* const);
bool parse_load_1(struct game* const);
bool parse_load_2(struct game* const);
bool parse_load_3(struct game* const);
bool parse_load_4(struct game* const);
bool parse_load_5(struct game* const);
bool parse_save_1(struct game* const);
bool parse_save_2(struct game* const);
bool parse_save_3(struct game* const);
bool parse_save_4(struct game* const);
bool parse_save_5(struct game* const);
bool parse_next_turn(struct game* const);
bool parse_pan_up(struct game* const);
bool parse_pan_left(struct game* const);
bool parse_pan_down(struct game* const);
bool parse_pan_right(struct game* const);
bool parse_action(struct game* const);
#endif
void parse_initialise(void);
bool parse_command(struct game* const, const uint8_t);

#endif
