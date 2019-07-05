#ifndef definitions_h
#define definitions_h

#include <stdint.h>

typedef uint8_t unit_index;
typedef uint8_t unit_health;
typedef uint8_t unit_type;
typedef uint8_t grid_index;
typedef uint16_t queue_index;
typedef uint16_t unit_energy;

#define queue_capacity 0xffff
#define units_capacity (unit_index)'\xff'
#define null_unit units_capacity
#define unit_index_format "%02X"
#define unit_type_format "%02X"
#define grid_index_format "%d"
#define grid_size 0x100
#define players_capacity 0x8
#define unit_player_offset (unit_index)'\x05'
#define unit_model_mask (unit_index)'\x1f'
// Number of items to print before and after ellipsis
#define verboseness 3
#define screen_width 8
#define screen_height 6
#define grid_width 8
#define grid_height 4
#define grid_capacity 10
#define unit_left 1
#define unit_top 1
#define unit_width 6
#define unit_height 2
#define selection_symbol '+'
#define selection_style '\xe0'
#define accessible_bit 1
#define attackable_bit 2
#define accessible_style '\xe0'
#define attackable_style '\x90'
#define both_style '\xc0'
#define units_models 15

// const static char* grid_names[grid_capacity] = {"void", "plains", "forest", "mountains", "beach", "sea", "reef", "river", "road", "bridge"};
const static uint8_t grid_symbols[grid_capacity] = {'.', '"', 'Y', '^', ':', '~', '*', ':', '-', '='};
const static uint8_t grid_styles[grid_capacity] = {'\x80', '\xA2', '\x32', '\x13', '\x3B', '\xC4', '\xD4', '\x4C', '\x78', '\x78'};
const static uint8_t unit_textures[units_models][unit_height][unit_width / 2] = {
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
const static uint8_t player_styles[players_capacity] = {'\xF8', '\xe6'};
const static uint8_t player_symbols[players_capacity] = {'1', '2'};
const static unit_energy unit_movement_ranges[units_models] = {3, 2, 8, 6, 5, 5, 5, 6, 4, 9, 7, 6, 5, 6, 5};
/*
0 int
1 mech
2 tires
3 tread
4 air
5 ships
*/

const static uint8_t unit_movement_types[units_models] = {0, 1, 2, 3, 3, 3, 2, 3, 2, 4, 4, 4, 5, 5, 5};
const static unit_energy movement_type_cost[6][grid_capacity] = {
	{0, 1, 1, 2, 1, 0, 0, 2, 1, 1},
	{0, 1, 1, 1, 1, 0, 0, 1, 1, 1},
	{0, 2, 3, 0, 1, 0, 0, 0, 1, 1},
	{0, 1, 2, 0, 1, 0, 0, 0, 1, 1},
	{0, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{0, 0, 0, 0, 0, 1, 2, 0, 0, 1}};

const static uint8_t grid_defense[6][grid_capacity] = {
	{0, 1, 2, 4, 0, 0, 0, 0, 1, 1},
	{0, 1, 2, 4, 0, 0, 0, 0, 1, 1},
	{0, 1, 2, 0, 0, 0, 0, 0, 0, 0},
	{0, 1, 2, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 1, 0, 0, 0}};

#endif
