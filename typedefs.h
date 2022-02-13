#ifndef typedefs_h
#define typedefs_h

#include <stdint.h>

typedef uint8_t grid_t;
typedef int16_t grid_wide_t;
typedef uint8_t tile_t;
typedef uint8_t player_t;

#define grid_size (grid_wide_t)256
#define model_capacity (uint8_t)15
#define movement_types_capacity (uint8_t)6
#define terrian_capacity (tile_t)10
#define capturable_capacity (tile_t)5
#define tile_capacity (terrian_capacity + capturable_capacity)
#define players_capacity (player_t)5
#define null_player players_capacity

#endif
