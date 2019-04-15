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
#define max_colours 0x8
#define unit_colour_offset (unit_index)'\x05'
#define unit_model_mask (unit_index)'\x1f'
// Number of items to print before and after ellipsis
#define verboseness 3


#endif
