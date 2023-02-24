#ifndef UNIT_CONSTANTS_H
#define UNIT_CONSTANTS_H

#include "game.h"

#define PASS_TYPES_CAPACITY 3

static const health_t pass_tile_defenses[PASS_TYPES_CAPACITY][TILE_CAPACITY] = {
    {0, 1, 2, 4, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 4},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 3, 0}};

static const health_t model_damages[MODEL_CAPACITY][MODEL_CAPACITY] = {
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

static const model_t building_buildable_models[BUILDING_CAPACITY + 1] = {
    0, 0, 9, 12, MODEL_CAPACITY, MODEL_CAPACITY};

#define CAPTURE_COMPLETION 200 // double HEALTH_MAX

typedef uint8_t movement_t;

static const movement_t model_movements[MODEL_CAPACITY] = {
    0, 1, 2, 3, 3, 3, 2, 3, 2, 4, 4, 4, 5, 5, 5};

typedef uint8_t pass_t;

static const uint8_t model_passes[MODEL_CAPACITY] = {0, 0, 0, 0, 0, 0, 0, 0,
                                                     0, 1, 1, 1, 2, 2, 2};

static const money_t model_costs[MODEL_CAPACITY] = {
    1000,  3000, 4000,  7000,  16000, 6000,  15000, 8000,
    12000, 9000, 22000, 20000, 20000, 18000, 28000};

static const energy_t model_movement_ranges[MODEL_CAPACITY] = {
    3, 2, 8, 6, 5, 5, 5, 6, 5, 6, 9, 7, 5, 6, 5};

static const energy_t
    movement_tile_costs[MOVEMENT_TYPES_CAPACITY][TILE_CAPACITY] = {
        {0, 1, 1, 2, 1, 0, 0, 2, 1, 1, 1, 1, 1, 1, 1},
        {0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1},
        {0, 2, 3, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1},
        {0, 1, 2, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1},
        {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {0, 0, 0, 0, 0, 1, 2, 0, 0, 1, 0, 0, 0, 1, 0}};

static const grid_t model_min_ranges[MODEL_CAPACITY] = {0, 0, 0, 0, 0, 2, 3, 0,
                                                        2, 0, 0, 0, 0, 0, 2};
static const grid_t model_max_ranges[MODEL_CAPACITY] = {0, 0, 0, 0, 0, 3, 6, 0,
                                                        5, 0, 0, 0, 0, 0, 7};

#define UNIT_BUILDING_UPPER_BOUND (model_t)2
#define MOVEMENT_TYPE_SHIP 5

#endif
