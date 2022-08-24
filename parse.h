#ifndef PARSE_H
#define PARSE_H

#include <stdbool.h>
#include <stdint.h>

#define KEY_BULID_1 '1'
#define KEY_BULID_2 '2'
#define KEY_BULID_3 '3'
#define KEY_BULID_4 '4'
#define KEY_BULID_5 '5'
#define KEY_BULID_6 '6'
#define KEY_BULID_7 '7'
#define KEY_BULID_8 '8'
#define KEY_BULID_9 '9'
#define KEY_BULID_10 '0'
#define KEY_LOAD_1 '!'
#define KEY_LOAD_2 '"'
#define KEY_LOAD_3 '#'
#define KEY_LOAD_4 '$'
#define KEY_LOAD_5 '%'
#define KEY_SAVE_1 '^'
#define KEY_SAVE_2 '&'
#define KEY_SAVE_3 '*'
#define KEY_SAVE_4 '('
#define KEY_SAVE_5 ')'
#define KEY_NEXT_TURN 'n'
#define KEY_PAN_UP 'w'
#define KEY_PAN_LFFT 'a'
#define KEY_PAN_DOWN 's'
#define KEY_PAN_RIGHT 'd'
#define KEY_HOVER_NEXT_UNIT 'm'
#define KEY_SELF_DESTRUCT 'k'
#define KEY_SURRENDER 'K'
#define KEY_ACTION ' '

struct game;

void parse_initialise(void);
bool parse_command(struct game* const, const uint8_t);

#endif
