#ifndef tyepdefs_h
#define tyepdefs_h

#include <stdint.h>

typedef uint8_t health_t;
typedef uint32_t health_wide_t;
typedef uint8_t model_t;
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
#define health_max (health_t)0xff
#define model_capacity 15
#define movement_types_capacity 6
#define terrian_capacity 10
#define capturable_capacity 5
#define tile_capacity (terrian_capacity + capturable_capacity)
#define units_capacity (unit_t)0xff
#define null_unit units_capacity
#define players_capacity 5
#define null_player players_capacity
#define heal_rate (health_t)51
#define defense_max (health_wide_t)10
#define attack_max (health_wide_t)100

#endif
