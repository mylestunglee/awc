#ifndef definitions_h
#define definitions_h

#include <stdint.h>

typedef uint8_t unit_t;
typedef uint8_t health_t;
typedef uint8_t model_t;
typedef uint8_t grid_t;
typedef uint16_t queue_t;
typedef uint16_t energy_t;
typedef uint8_t tile_t;
typedef uint32_t health_t_wide;
typedef uint8_t player_t;

#define health_max 0xff
#define queue_capacity 0xffff
#define units_capacity (unit_t)'\xff'
#define null_unit units_capacity
#define unit_t_format "%02X"
#define unit_type_format "%02X"
#define grid_t_format "%d"
#define grid_size 0x100
#define players_capacity 0x8
#define unit_player_offset (unit_t)'\x05'
#define model_t_mask (unit_t)'\x1f'
#define screen_width 10
#define screen_height 8
#define tile_width 8
#define tile_height 4
#define tile_capacity 10
#define unit_left 1
#define unit_top 1
#define unit_width 5
#define unit_height 2
#define selection_symbol '+'
#define accessible_bit 1
#define attackable_bit 2
#define accessible_style '\xe0'
#define attackable_style '\x90'
#define model_capacity 15

// const static char* grid_names[tile_capacity] = {"void", "plains", "forest", "mountains", "beach", "sea", "reef", "river", "road", "bridge"};
const static uint8_t grid_symbols[tile_capacity] = {'.', '"', 'Y', '^', ':', '~', '*', ':', '-', '='};
const static uint8_t grid_styles[tile_capacity] = {'\x80', '\xA2', '\x32', '\x13', '\x3B', '\xC4', '\xD4', '\x4C', '\x78', '\x78'};
const static uint8_t unit_textures[model_capacity][unit_height][(unit_width + 1) / 2] = {
	{	{'\x03', '\xF3', '\x00'},
		{'\x08', '\x88', '\x00'}
	}, {{'\x0E', '\xFE', '\x00'},
		{'\x08', '\x88', '\x00'}
	}, {{'\x0A', '\xFD', '\x00'},
		{'\x03', '\x33', '\x00'}
	}, {{'\xAB', '\xFC', '\xD0'},
		{'\x3E', '\xEE', '\x30'}
	}, {{'\xBB', '\xFC', '\xE0'},
		{'\x3E', '\xEE', '\x30'}
	}, {{'\x08', '\xF8', '\x00'},
		{'\x03', '\xE3', '\x00'}
	}, {{'\x88', '\xF8', '\x80'},
		{'\x33', '\x33', '\x30'}
	}, {{'\x0A', '\xFA', '\x00'},
		{'\x03', '\xE3', '\x00'}
	}, {{'\xAA', '\xFA', '\xA0'},
		{'\x33', '\x33', '\x30'}
	}, {{'\x91', '\x11', '\x10'},
		{'\x59', '\xFC', '\xE0'}
	}, {{'\x91', '\x11', '\x10'},
		{'\x57', '\xF7', '\x60'}
	}, {{'\x0E', '\x4E', '\x10'},
		{'\x0B', '\xFC', '\xE0'}
	}, {{'\x88', '\xF8', '\x80'},
		{'\x92', '\x22', '\xA0'}
	}, {{'\x2A', '\xFA', '\x20'},
		{'\x92', '\x2C', '\xE0'}
	}, {{'\x12', '\x22', '\x10'},
		{'\x5E', '\xFC', '\xE0'}}};

const static uint8_t unit_symbols[14] = {' ', '_', 'o', 'x', '<', '>', 'v', '^', '\\', '/', '[', ']', '-', '='};
const static uint8_t player_styles[players_capacity] = {'\xF8', '\xF6'};
const static uint8_t player_symbols[players_capacity] = {'1', '2'};
const static energy_t unit_movement_ranges[model_capacity] = {3, 2, 8, 6, 5, 5, 5, 6, 4, 9, 7, 6, 5, 6, 5};
/*
0 int
1 mech
2 tires
3 tread
4 air
5 ships
*/

#define movement_type_ship 5
#define tile_bridge 9

const static uint8_t unit_movement_types[model_capacity] = {0, 1, 2, 3, 3, 3, 2, 3, 2, 4, 4, 4, 5, 5, 5};
const static energy_t movement_type_cost[6][tile_capacity] = {
	{0, 1, 1, 2, 1, 0, 0, 2, 1, 1},
	{0, 1, 1, 1, 1, 0, 0, 1, 1, 1},
	{0, 2, 3, 0, 1, 0, 0, 0, 1, 1},
	{0, 1, 2, 0, 1, 0, 0, 0, 1, 1},
	{0, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{0, 0, 0, 0, 0, 1, 2, 0, 0, 1}};

const static uint8_t grid_defense[6][tile_capacity] = {
	{0, 1, 2, 4, 0, 0, 0, 0, 1, 1},
	{0, 1, 2, 4, 0, 0, 0, 0, 1, 1},
	{0, 1, 2, 0, 0, 0, 0, 0, 0, 0},
	{0, 1, 2, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 1, 0, 0, 0}};

const static uint8_t units_damage[model_capacity][model_capacity] = {
	{55,  45,  12,  5,   1,   15,  25,  5,   25,  0,   0,   7,   0,   0,   0},
	{65,  55,  85,  55,  15,  70,  85,  65,  85,  0,   0,   9,   0,   0,   0},
	{70,  65,  35,  6,   1,   45,  55,  4,   28,  0,   0,   10,  0,   0,   0},
	{75,  70,  85,  55,  15,  70,  85,  65,  85,  0,   0,   10,  1,   5,   1},
	{105, 95,  105, 85,  55,  105, 105, 105, 105, 0,   0,   12,  10,  45,  10},
	{90,  85,  80,  70,  45,  75,  80,  75,  80,  0,   0,   0,   40,  65,  60},
	{95,  90,  90,  85,  55,  80,  85,  85,  90,  0,   0,   0,   55,  85,  85},
	{105, 105, 60,  25,  10,  50,  45,  45,  55,  65,  75,  120, 0,   0,   0},
	{0,   0,   0,   0,   0,   0,   0,   0,   0,   100, 100, 120, 0,   0,   0},
	{0,   0,   0,   0,   0,   0,   0,   0,   0,   55,  100, 100, 0,   0,   0},
	{110, 110, 105, 105, 95,  105, 105, 95,  105, 0,   0,   0,   75,  85,  95},
	{75,  75,  55,  55,  25,  65,  65,  25,  65,  0,   0,   65,  25,  55,  25},
	{95,  90,  90,  85,  55,  80,  85,  85,  90,  0,   0,   0,   50,  95,  95},
	{0,   0,   0,   0,   0,   0,   0,   0,   0,   55,  65,  115, 0,    0,  90},
	{0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   55,  25,  55}
};

#endif

