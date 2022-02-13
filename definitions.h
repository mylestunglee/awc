#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <stdint.h>

typedef uint8_t grid_t;
typedef int16_t grid_wide_t;
typedef uint8_t tile_t;
typedef uint8_t player_t;

#define GRID_SIZE (grid_wide_t)256
#define MODEL_CAPACITY (uint8_t)15
#define MOVEMENT_TYPES_CAPACITY (uint8_t)6
#define TERRIAN_CAPACITY (tile_t)10
#define CAPTURABLE_CAPACITY (tile_t)5
#define TILE_CAPACITY (TERRIAN_CAPACITY + CAPTURABLE_CAPACITY)
#define PLAYERS_CAPACITY (player_t)5

#endif
