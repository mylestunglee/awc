#ifndef BAP_H
#define BAP_H

#include "game.h"

struct bap_inputs {
    health_wide_t friendly_distribution[MODEL_CAPACITY];
    health_wide_t enemy_distribution[MODEL_CAPACITY];
    grid_wide_t capturables[CAPTURABLE_CAPACITY];
    money_t budget;
};

#endif
