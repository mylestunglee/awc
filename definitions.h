#ifndef definitions_h
#define definitions_h

#include <stdint.h>

// Types

typedef uint8_t unit_t;
typedef uint8_t health_t;
typedef uint32_t health_wide_t;
typedef uint8_t model_t;
typedef uint8_t grid_t;
typedef int16_t grid_wide_t;
typedef uint16_t list_t;
typedef uint16_t energy_t;
typedef uint8_t tile_t;
typedef uint16_t tile_wide_t;
typedef uint8_t player_t;
typedef uint16_t player_wide_t;
typedef int32_t gold_t;

#define grid_size (grid_wide_t)256
#define health_max (health_t)0xff
#define model_capacity 15
#define movement_types_capacity 6
#define terrian_capacity 10
#define capturable_capacity 5
#define tile_capacity (terrian_capacity + capturable_capacity)
#define list_capacity 0x10000
#define units_capacity (unit_t)0xff
#define null_unit units_capacity
#define players_capacity 5
#define null_player players_capacity
#define heal_rate (health_t)51
#define defense_max (health_wide_t)10
#define attack_max (health_wide_t)100

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

// Graphics
#define tile_width 8
#define tile_height 4
#define unit_left 1
#define unit_top 1
#define unit_width 5
#define unit_height 2

#define selection_symbol '+'
#define accessible_bit 1
#define attackable_bit 2
#define bot_target_bit 4
#define accessible_style '\xe0'
#define attackable_style '\x90'
#define accessible_attackable_style '\xd0'
#define buildable_style '\xf0'

const static char* const tile_names[tile_capacity] = {"void", "plains", "forest", "mountains", "beach", "sea", "reef", "river", "road", "bridge", "city", "factory", "airport", "habour", "HQ"};
const static char* const model_names[model_capacity] = {"infantry", "mech", "recon", "tank", "battletank", "artillery", "rockets", "antiair", "missles", "battlecopter", "fighter", "bomber", "submarine", "crusier", "battleship"};
const static uint8_t tile_symbols[tile_capacity] = {'.', '"', 'Y', '^', ':', '~', '*', '\'', '-', '=', 'C', 'F', 'A', 'S', 'H'};
const static uint8_t tile_styles[terrian_capacity] = {'\x80', '\xA2', '\x32', '\x13', '\x3B', '\xC4', '\xD4', '\x4C', '\x78', '\x78'};
const static uint8_t unit_textures[model_capacity][unit_height][(unit_width + 1) / 2] = {
	{	{'\x03', '\xF3', '\x00'},
		{'\x08', '\x88', '\x00'}
	}, {	{'\x0E', '\xFE', '\x00'},
		{'\x08', '\x88', '\x00'}
	}, {	{'\x0A', '\xFD', '\x00'},
		{'\x03', '\x33', '\x00'}
	}, {	{'\xAB', '\xFC', '\xD0'},
		{'\x3E', '\xEE', '\x30'}
	}, {	{'\xB1', '\xFC', '\xE0'},
		{'\x3E', '\xEE', '\x30'}
	}, {	{'\x08', '\xF8', '\x00'},
		{'\x03', '\xE3', '\x00'}
	}, {	{'\x88', '\xF8', '\x80'},
		{'\x33', '\x33', '\x30'}
	}, {	{'\x0A', '\xFA', '\x00'},
		{'\x03', '\xE3', '\x00'}
	}, {	{'\xAA', '\xFA', '\xA0'},
		{'\x33', '\x33', '\x30'}
	}, {	{'\x0E', '\x4E', '\x10'},
		{'\x0B', '\xFC', '\xE0'}
	}, {	{'\x91', '\x11', '\x10'},
		{'\x59', '\xFC', '\xE0'}
	}, {	{'\x91', '\x11', '\x10'},
		{'\x57', '\xF7', '\x60'}
	}, {	{'\x22', '\x22', '\x20'},
		{'\x92', '\xFC', '\xE0'}
	}, {	{'\x2A', '\xFA', '\x20'},
		{'\x92', '\x2C', '\xE0'}
	}, {	{'\x88', '\xF8', '\x80'},
		{'\x92', '\x22', '\xA0'}}};

const static uint8_t capturable_textures[capturable_capacity][tile_height][(tile_width + 1) / 2] = {
	{	{'\x00', '\xB1', '\xFC', '\x00'},
		{'\x00', '\xB1', '\xB1', '\x1C'},
		{'\xB1', '\x1C', '\xB1', '\x1C'},
		{'\xB1', '\x1C', '\x00', '\x00'},
	}, {	{'\x00', '\x00', '\x00', '\x00'},
		{'\xB9', '\x2B', '\xF2', '\xAC'},
		{'\xB1', '\x11', '\x11', '\x1C'},
		{'\xB1', '\x11', '\x11', '\x1C'},
	}, {	{'\x00', '\x00', '\x9F', '\xA0'},
		{'\x00', '\x00', '\xC1', '\xB0'},
		{'\x22', '\x22', '\xC1', '\xB2'},
		{'\xB1', '\x11', '\x11', '\x1C'},
	}, {	{'\x00', '\x00', '\x00', '\x00'},
		{'\x00', '\x00', '\xBC', '\xFC'},
		{'\x22', '\x22', '\xBC', '\x1C'},
		{'\xB1', '\x11', '\x11', '\x1C'},
	}, {	{'\x00', '\xBE', '\xFC', '\x00'},
		{'\x00', '\xB1', '\x1C', '\x00'},
		{'\xB1', '\x11', '\x11', '\x1C'},
		{'\xB1', '\x11', '\x11', '\x1C'}}};

const static uint8_t unit_symbols[14] = {' ', '_', 'o', 'x', '<', '>', 'v', '^', '\\', '/', '[', ']', '-', '='};
const static uint8_t player_styles[players_capacity + 1] = {'\xF4', '\xF1', '\xF3', '\xF8', '\xF8', '\xF8'};
const static uint8_t player_symbols[players_capacity + 1] = {'1', '2', '3', '4', '5', ' '};
const static energy_t unit_movement_ranges[model_capacity] = {3, 2, 8, 6, 5, 5, 5, 6, 5, 6, 9, 7, 5, 6, 5};
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
#define tile_forest 2
#define tile_mountain 3
#define tile_bridge 9
#define tile_city 10
#define tile_HQ 14

const static uint8_t unit_movement_types[model_capacity] = {0, 1, 2, 3, 3, 3, 2, 3, 2, 4, 4, 4, 5, 5, 5};
const static energy_t movement_type_cost[movement_types_capacity][tile_capacity] = {
	{0, 1, 1, 2, 1, 0, 0, 2, 1, 1, 1, 1, 1, 1, 1},
	{0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1},
	{0, 2, 3, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1},
	{0, 1, 2, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1},
	{0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{0, 0, 0, 0, 0, 1, 2, 0, 0, 1, 0, 0, 0, 1, 0}};

const static health_t tile_defense[movement_types_capacity][tile_capacity] = {
	{0, 1, 2, 4, 0, 0, 0, 0, 1, 1, 3, 3, 3, 3, 4},
	{0, 1, 2, 4, 0, 0, 0, 0, 1, 1, 3, 3, 3, 3, 4},
	{0, 1, 2, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 4},
	{0, 1, 2, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 4},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 3, 0}};

const static health_t units_damage[model_capacity][model_capacity] = {
	{55,  45,  12,  5,   1,   15,  25,  5,   25,  7,   0,   0,   0,   0,   0  },
	{65,  55,  85,  55,  15,  70,  85,  65,  85,  9,   0,   0,   0,   0,   0  },
	{70,  65,  35,  6,   1,   45,  55,  4,   28,  10,  0,   0,   0,   0,   0  },
	{75,  70,  85,  55,  15,  70,  85,  65,  85,  10,  0,   0,   1,   5,   1  },
	{105, 95,  105, 85,  55,  105, 105, 105, 105, 12,  0,   0,   10,  45,  10 },
	{90,  85,  80,  70,  45,  75,  80,  75,  80,  0,   0,   0,   60,  65,  40 },
	{95,  90,  90,  85,  55,  80,  85,  85,  90,  0,   0,   0,   85,  85,  55 },
	{105, 105, 60,  25,  10,  50,  45,  45,  55,  120, 65,  75,  0,   0,   0  },
	{0,   0,   0,   0,   0,   0,   0,   0,   0,   120, 100, 100, 0,   0,   0  },
	{75,  75,  55,  55,  25,  65,  65,  25,  65,  65,  0,   0,   25,  55,  25 },
	{0,   0,   0,   0,   0,   0,   0,   0,   0,   100, 55,  100, 0,   0,   0  },
	{110, 110, 105, 105, 95,  105, 105, 95,  105, 0,   0,   0,   95,  85,  75 },
	{0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   55,  25,  55 },
	{0,   0,   0,   0,   0,   0,   0,   0,   0,   115, 55,  65,  90,  0,   0  },
	{95,  90,  90,  85,  55,  80,  85,  85,  90,  0,   0,   0,   95,  95,  50 }};

const static grid_t models_min_range[model_capacity] = {0, 0, 0, 0, 0, 1, 2, 0, 2, 0, 0, 0, 0, 0, 2};
const static grid_t models_max_range[model_capacity] = {0, 0, 0, 0, 0, 2, 5, 0, 5, 0, 0, 0, 0, 0, 6};
const static gold_t models_cost[model_capacity] = {1, 3, 4, 7, 16, 6, 15, 8, 12, 9, 22, 20, 20, 18, 28};
const static model_t buildable_models[capturable_capacity + 1] = {0, 0, 9, 12, model_capacity, model_capacity};

#define gold_scale 10
#define bitarray_size(bits) (bits + 7) / 8

#endif
