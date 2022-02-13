#ifndef typedefs_h
#define typedefs_h

#include <stdint.h>

typedef uint8_t grid_t;
typedef int16_t grid_wide_t;
typedef uint16_t energy_t;
typedef uint8_t tile_t;
typedef uint16_t tile_wide_t;
typedef uint8_t player_t;
typedef uint16_t player_wide_t;
typedef int32_t gold_t;
typedef uint8_t movement_t;

#define grid_size (grid_wide_t)256
#define model_capacity 15
#define movement_types_capacity 6
#define terrian_capacity 10
#define capturable_capacity 5
#define tile_capacity (terrian_capacity + capturable_capacity)
#define players_capacity 5
#define null_player players_capacity

#endif
