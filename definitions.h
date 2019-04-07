#ifndef definitions_h
#define definitions_h

#include <stdint.h>

typedef uint8_t unit_index;
typedef uint8_t unit_health;
typedef uint8_t unit_type;

typedef uint8_t grid_index;

#define units_capacity (unit_index)'\xff'
#define unit_index_show_format "%02x"
#define unit_index_hide_format "__"
#define unit_type_format "%02x"
#define grid_index_format "%d"
#define grid_size 0x100
#define max_colours 0x8
#define unit_colour_offset (unit_index)'\x05'

#endif
