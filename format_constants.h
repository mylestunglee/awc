#ifndef FORMAT_CONSTANTS_H
#define FORMAT_CONSTANTS_H

#include "definitions.h"

#define UNIT_FORMAT "%02X"
#define GRID_FORMAT "%3hhu"
#define PLAYER_FORMAT "%3hhu"
#define HEALTH_FORMAT "%4hhu"
#define ROW_FORMAT "%256s"
#define MODEL_NAME_FORMAT "%-12s"
#define MODEL_FORMAT "%hhu"
#define TURN_FORMAT "%hhu"
#define HEALTH_WIDE_FORMAT "%hu"
#define GOLD_FORMAT "%d"
#define ENABLED_FORMAT "%8s"
#define TILE_FORMAT "%hhu"

const static uint8_t tile_symbols[TILE_CAPACITY] = {
    '.', '"', 'Y', '^', ':', '~', '*', '\'', '-', '=', 'C', 'F', 'A', 'S', 'H'};

const static char* const model_names[MODEL_CAPACITY] = {
    "infantry",  "mech",    "recon",     "tank",    "battletank",
    "artillery", "rockets", "antiair",   "missles", "battlecopter",
    "fighter",   "bomber",  "submarine", "crusier", "battleship"};

const static char* const tile_names[TILE_CAPACITY] = {
    "void", "plains",  "forest",  "mountains", "beach",
    "sea",  "reef",    "river",   "road",      "bridge",
    "city", "factory", "airport", "habour",    "HQ"};

#endif
