#ifndef PARSE_H
#define PARSE_H

#include "game.h"

#define KEY_QUIT 'q'
#define KEY_NEXT_TURN 'n'
#define KEY_PAN_UP 'w'
#define KEY_PAN_LFFT 'a'
#define KEY_PAN_DOWN 's'
#define KEY_PAN_RIGHT 'd'
#define KEY_HOVER_NEXT_UNIT 'm'
#define KEY_SELF_DESTRUCT 'k'
#define KEY_SURRENDER 'K'
#define KEY_ACTION ' '

bool parse_command(struct game* const, const char);

#endif
