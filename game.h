#ifndef game_h
#define game_h

#include "list.h"
#include "units.h"

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

void game_simulate_attack(const struct game* const, health_t* const,
                          health_t* const);

bool game_select_next_unit(struct game* const);
void game_handle_unit_selection(struct game* const);
void game_print_text(const struct game* const, const bool, const bool);
bool game_attack_enabled(const struct game* const);
bool game_build_enabled(const struct game* const);
bool game_is_alive(const struct game* const, const player_t);
bool game_is_bot(const struct game* const, const player_t);
void game_remove_player(struct game* const, const player_t);

#endif
