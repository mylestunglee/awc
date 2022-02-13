#ifndef unit_constants_h
#define unit_constants_h

#include "game.h"

const static health_t tile_defense[movement_types_capacity][tile_capacity] = {
    {0, 1, 2, 4, 0, 0, 0, 0, 1, 1, 3, 3, 3, 3, 4},
    {0, 1, 2, 4, 0, 0, 0, 0, 1, 1, 3, 3, 3, 3, 4},
    {0, 1, 2, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 4},
    {0, 1, 2, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 4},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 3, 0}};

const static health_t units_damage[model_capacity][model_capacity] = {
    {55, 45, 12, 5, 1, 15, 25, 5, 25, 7, 0, 0, 0, 0, 0},
    {65, 55, 85, 55, 15, 70, 85, 65, 85, 9, 0, 0, 0, 0, 0},
    {70, 65, 35, 6, 1, 45, 55, 4, 28, 10, 0, 0, 0, 0, 0},
    {75, 70, 85, 55, 15, 70, 85, 65, 85, 10, 0, 0, 0, 5, 1},
    {105, 95, 105, 85, 55, 105, 105, 105, 105, 12, 0, 0, 0, 45, 10},
    {90, 85, 80, 70, 45, 75, 80, 75, 80, 0, 0, 0, 0, 65, 40},
    {95, 90, 90, 85, 55, 80, 85, 85, 90, 0, 0, 0, 0, 85, 55},
    {105, 105, 60, 25, 10, 50, 45, 45, 55, 120, 65, 75, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 120, 100, 100, 0, 0, 0},
    {75, 75, 55, 55, 25, 65, 65, 25, 65, 65, 0, 0, 0, 55, 25},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 100, 55, 100, 0, 0, 0},
    {110, 110, 105, 105, 95, 105, 105, 95, 105, 0, 0, 0, 0, 85, 75},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 55, 25, 55},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 115, 55, 65, 90, 0, 0},
    {95, 90, 90, 85, 55, 80, 85, 85, 90, 0, 0, 0, 0, 95, 50}};

const static model_t buildable_models[capturable_capacity + 1] = {
    0, 0, 9, 12, model_capacity, model_capacity};

#define capture_completion (capture_progress_t)0x1fe // double health_max

typedef uint8_t movement_t;

const static movement_t unit_movement_types[model_capacity] = {
    0, 1, 2, 3, 3, 3, 2, 3, 2, 4, 4, 4, 5, 5, 5};
const static uint8_t unit_pass_type[model_capacity] = {0, 0, 0, 0, 0, 0, 0, 0,
                                                       0, 1, 1, 1, 2, 2, 2};

const static gold_t models_cost[model_capacity] = {
    1000,  3000, 4000,  7000,  16000, 6000,  15000, 8000,
    12000, 9000, 22000, 20000, 20000, 18000, 28000};

const static energy_t unit_movement_ranges[model_capacity] = {
    3, 2, 8, 6, 5, 5, 5, 6, 5, 6, 9, 7, 5, 6, 5};

const static energy_t
    movement_type_cost[movement_types_capacity][tile_capacity] = {
        {0, 1, 1, 2, 1, 0, 0, 2, 1, 1, 1, 1, 1, 1, 1},
        {0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1},
        {0, 2, 3, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1},
        {0, 1, 2, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1},
        {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {0, 0, 0, 0, 0, 1, 2, 0, 0, 1, 0, 0, 0, 1, 0}};

const static grid_t models_min_range[model_capacity] = {0, 0, 0, 0, 0, 2, 3, 0,
                                                        2, 0, 0, 0, 0, 0, 2};
const static grid_t models_max_range[model_capacity] = {0, 0, 0, 0, 0, 3, 6, 0,
                                                        5, 0, 0, 0, 0, 0, 7};

#define unit_capturable_upper_bound (model_t)2
#define movement_type_ship 5

#endif