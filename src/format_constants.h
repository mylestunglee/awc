#ifndef FORMAT_CONSTANTS_H
#define FORMAT_CONSTANTS_H

#include "definitions.h"

#define UNIT_FORMAT "%02X"
#define GRID_FORMAT "%3hhu"
#define PLAYER_FORMAT "%hhu"
#define HEALTH_FORMAT "%3hhu"
#define ROW_FORMAT "%256s"
#define MODEL_NAME_FORMAT "%-12s"
#define MODEL_FORMAT "%hhu"
#define HEALTH_WIDE_FORMAT "%hu"
#define MONEY_FORMAT "%i"
#define ENABLED_FORMAT "%8s"
#define TILE_FORMAT "%hhu"
#define CAPTURE_COMPLETION_FORMAT "%hhu"

static const uint8_t tile_symbols[TILE_CAPACITY] = {
    '.', '"', 'Y', '^', ':', '~', '*', '\'', '-', '=', 'C', 'F', 'A', 'S', 'H'};

static const char* const model_names[MODEL_CAPACITY] = {
    "infantry",  "mech",    "recon",     "tank",    "battletank",
    "artillery", "rockets", "antiair",   "missles", "battlecopter",
    "fighter",   "bomber",  "submarine", "crusier", "battleship"};

#endif
