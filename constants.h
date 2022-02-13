#ifndef constants_h
#define constants_h

#include "typedefs.h"
#include "list.h"

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

const static energy_t unit_movement_ranges[model_capacity] = {
    3, 2, 8, 6, 5, 5, 5, 6, 5, 6, 9, 7, 5, 6, 5};
/*
0 int
1 mech
2 tires
3 tread
4 air
5 ships
*/

#define unit_capturable_upper_bound 2
#define movement_type_ship 5
#define tile_void (tile_t)0
#define tile_plains (tile_t)1
#define tile_forest (tile_t)2
#define tile_mountain (tile_t)3
#define tile_bridge (tile_t)9
#define tile_city (tile_t)10
#define tile_factory (tile_t)11
#define tile_hq (tile_t)14

const static movement_t unit_movement_types[model_capacity] = {
    0, 1, 2, 3, 3, 3, 2, 3, 2, 4, 4, 4, 5, 5, 5};
const static uint8_t unit_pass_type[model_capacity] = {0, 0, 0, 0, 0, 0, 0, 0,
                                                       0, 1, 1, 1, 2, 2, 2};
const static energy_t
    movement_type_cost[movement_types_capacity][tile_capacity] = {
        {0, 1, 1, 2, 1, 0, 0, 2, 1, 1, 1, 1, 1, 1, 1},
        {0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1},
        {0, 2, 3, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1},
        {0, 1, 2, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1},
        {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {0, 0, 0, 0, 0, 1, 2, 0, 0, 1, 0, 0, 0, 1, 0}};



const static grid_t models_min_range[model_capacity] = {0, 0, 0, 0, 0, 2, 3, 0,
                                                        2, 0, 0, 0, 0, 0, 2};
const static grid_t models_max_range[model_capacity] = {0, 0, 0, 0, 0, 3, 6, 0,
                                                        5, 0, 0, 0, 0, 0, 7};
const static gold_t models_cost[model_capacity] = {1,  3, 4,  7,  16, 6,  15, 8,
                                                   12, 9, 22, 20, 20, 18, 28};

#define gold_scale (gold_t)1000

#endif
