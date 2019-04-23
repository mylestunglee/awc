#ifndef definitions_h
#define definitions_h

#include <stdint.h>

typedef uint8_t unit_index;
typedef uint8_t unit_health;
typedef uint8_t unit_type;

typedef uint8_t grid_index;

typedef uint8_t tile_index;

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
#define screen_width 4
#define screen_height 3
#define tile_width 8
#define tile_height 4
#define tile_count 10
#define unit_left 1
#define unit_top 1
#define unit_width 6
#define unit_height 3
#define unit_right unit_left + unit_width
#define unit_bottom unit_top + unit_height

const static uint8_t tile_symbols[tile_count] = {'`', '^', '-', 'x'};
const static uint8_t tile_styles[tile_count] = {'\xA2', '\x23', '\x78'};
const static uint8_t unit_textures[1][tile_height][(tile_width + 1) / 2] = {
	{	{'\x01', '\x32', '\x03'},
		{'\xf2', '\x24', '\x03'},
		{'\x01', '\x1f', '\x03'}
	}};
const static uint8_t unit_symbols[16] = {'a', 'b', 'd', 'e', 'f', 'g'};
const static uint8_t player_style[players_capacity] = {'\xf7', '\xe6'};
const static uint8_t player_symbol[players_capacity] = {'W', 'E'};

#endif
