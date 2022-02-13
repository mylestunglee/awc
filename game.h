#ifndef game_h
#define game_h

#include "list.h"
#include "units.h"

#define bitarray_size(bits) (bits + 7) / 8

struct game {
    tile_t map[grid_size][grid_size];
    struct units units;
    grid_t x;
    grid_t y;
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
