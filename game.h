#ifndef GAME_H
#define GAME_H

#include "bitarray.h"
#include "list.h"
#include "units.h"

typedef int32_t gold_t;

struct game {
    struct units units;
    struct list list;
    gold_t golds[PLAYERS_CAPACITY];
    gold_t incomes[PLAYERS_CAPACITY];
    energy_t energies[GRID_SIZE][GRID_SIZE];
    tile_t map[GRID_SIZE][GRID_SIZE];
    uint8_t labels[GRID_SIZE][GRID_SIZE];
    uint8_t bots[BITARRAY_SIZE(PLAYERS_CAPACITY)];
    uint8_t alliances[BITMATRIX_SIZE(PLAYERS_CAPACITY)];
    player_t territory[GRID_SIZE][GRID_SIZE];
    player_t turn;
    grid_t x;
    grid_t y;
    grid_t prev_x;
    grid_t prev_y;
    bool dirty_labels;
};

void game_initialise(struct game* const);
#ifdef EXPOSE_GAME_INTERNALS
bool are_turns_empty(const struct game* const);
void skip_turns(struct game* const);
void skip_empty_turns(struct game* const);
#endif
bool game_load(struct game* const, const char* const);
bool game_save(struct game* const, const char* const);
void game_deselect(struct game* const);
#ifdef EXPOSE_GAME_INTERNALS
health_t calc_damage(const struct game* const, const struct unit* const,
                     const struct unit* const);
void calc_damage_pair(const struct game* const, const struct unit* const,
                      const struct unit* const, health_t* const,
                      health_t* const);
void calc_damage_pair_with_health(const struct game* const,
                                  const struct unit* const,
                                  const struct unit* const, const health_t,
                                  health_t* const, health_t* const);
#endif
void game_calc_damage(const struct game* const, health_t* const,
                      health_t* const);
bool game_is_attackable(const struct game* const);
bool game_is_buildable(const struct game* const);
bool game_is_alive(const struct game* const, const player_t);
bool game_is_bot(const struct game* const, const player_t);
bool game_is_friendly(const struct game* const, const player_t);
void game_remove_player(struct game* const, const player_t);

#endif
