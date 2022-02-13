#ifndef format_constants_h
#define format_constants_h

#include "typedefs.h"

#define unit_format "%02X"
#define grid_format "%3hhu"
#define player_format "%3hhu"
#define health_format "%4hhu"
#define row_format "%256s"
#define model_name_format "%-12s"
#define model_format "%hhu"
#define turn_format "%hhu"
#define health_wide_format "%u"
#define gold_format "%d"
#define enabled_format "%8s"
#define tile_format "%hhu"

const static uint8_t tile_symbols[tile_capacity] = {
    '.', '"', 'Y', '^', ':', '~', '*', '\'', '-', '=', 'C', 'F', 'A', 'S', 'H'};

const static char* const model_names[model_capacity] = {
    "infantry",  "mech",    "recon",     "tank",    "battletank",
    "artillery", "rockets", "antiair",   "missles", "battlecopter",
    "fighter",   "bomber",  "submarine", "crusier", "battleship"};

const static char* const tile_names[tile_capacity] = {
    "void", "plains",  "forest",  "mountains", "beach",
    "sea",  "reef",    "river",   "road",      "bridge",
    "city", "factory", "airport", "habour",    "HQ"};

#endif
