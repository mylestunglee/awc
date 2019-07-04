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
#define unit_height 3
#define selection_symbol '+'
#define selection_style '\xe0'
#define accessible_bit 1
#define attackable_bit 2
#define accessible_style '\xe0'
#define attackable_style '\x90'
#define both_style '\xc0'

// const static char* grid_names[grid_capacity] = {"void", "plains", "forest", "mountains", "beach", "sea", "reef", "river", "road", "bridge"};
const static uint8_t grid_symbols[grid_capacity] = {' ', '"', 'Y', '^', ':', '~', '*', ':', '-', '='};
const static uint8_t grid_styles[grid_capacity] = {'\x00', '\xA2', '\x32', '\x13'};
const static uint8_t unit_textures[1][unit_height][unit_width / 2] = {
	{	{'\x01', '\x32', '\x03'},
		{'\xf2', '\x24', '\x03'},
		{'\x01', '\x1f', '\x03'}
	}};
const static uint8_t unit_symbols[16] = {'a', 'b', 'd', 'e', 'f', 'g'};
const static uint8_t player_style[players_capacity] = {'\xf7', '\xe6'};
const static uint8_t player_symbol[players_capacity] = {'W', 'E'};

#endif
