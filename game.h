#ifndef game_h
#define game_h

#include "list.h"
#include "units.h"

struct game {
    tile_t map[grid_size][grid_size];
    struct units units;
    grid_t x;
    grid_t y;
    unit_t selected;
    uint8_t labels[grid_size][grid_size];
    energy_t energies[grid_size][grid_size];
    struct list list;
    grid_t prev_x;
    grid_t prev_y;
    player_t turn;
    player_t territory[grid_size][grid_size];
    gold_t golds[players_capacity];
    gold_t incomes[players_capacity];
    uint8_t bots[bitarray_size(players_capacity)];
    uint8_t alliances[bitarray_size(
        (players_capacity * (players_capacity - 1)) / 2)];
};

bool game_load(struct game* const, const char* const);
void game_loop(struct game* const);

void game_simulate_attack(const struct game* const game, health_t* const damage,
                          health_t* const counter_damage);

#endif
