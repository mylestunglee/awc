#ifndef constants_h
#define constants_h

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

#define accessible_bit 1
#define attackable_bit 2

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

#define movement_type_ship 5
#define tile_void (tile_t)0
#define tile_plains (tile_t)1
#define tile_forest (tile_t)2
#define tile_mountain (tile_t)3
#define tile_bridge (tile_t)9
#define tile_city (tile_t)10
#define tile_factory (tile_t)11
#define tile_hq (tile_t)14

#endif
