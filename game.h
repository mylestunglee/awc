#ifndef GAME_H
#define GAME_H

#include "list.h"
#include "units.h"

#define BITARRAY_SIZE(bits) (bits + 7) / 8

typedef int32_t gold_t;

struct game {
    tile_t map[GRID_SIZE][GRID_SIZE];
    struct units units;
    grid_t x;
    grid_t y;
    uint8_t labels[GRID_SIZE][GRID_SIZE];
    energy_t energies[GRID_SIZE][GRID_SIZE];
    struct list list;
    grid_t prev_x;
    grid_t prev_y;
    player_t turn;
    player_t territory[GRID_SIZE][GRID_SIZE];
    gold_t golds[PLAYERS_CAPACITY];
    gold_t incomes[PLAYERS_CAPACITY];
    uint8_t bots[BITARRAY_SIZE(PLAYERS_CAPACITY)];
    uint8_t alliances[BITARRAY_SIZE(
        (PLAYERS_CAPACITY * (PLAYERS_CAPACITY - 1)) / 2)];
    bool dirty_labels;
};

void game_initialise(struct game* const);
bool game_load(struct game* const, const char* const);
void game_deselect(struct game* const);
#ifdef expose_game_internals
const struct unit* find_next_unit(const struct game* const);
#endif
bool game_hover_next_unit(struct game* const);
#ifdef expose_game_internals
health_t calc_damage(const struct game* const, const struct unit* const,
                     const struct unit* const);
#endif
void game_simulate_attack(const struct game* const, health_t* const,
                          health_t* const);
bool game_is_attackable(const struct game* const);
bool game_is_buildable(const struct game* const);
bool game_is_alive(const struct game* const, const player_t);
bool game_is_bot(const struct game* const, const player_t);
bool game_is_friendly(const struct game* const, const player_t);
void game_remove_player(struct game* const, const player_t);

#endif
