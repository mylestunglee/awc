#ifndef FILE_H
#define FILE_H

#include "game.h"
#include <stdbool.h>
#include <stdio.h>

#ifdef EXPOSE_FILE_INTERNALS
bool load_turn(const char* const, const char* const, player_t* const);
bool load_map(const char* const, const char* const, grid_t* const,
              tile_t[GRID_SIZE][GRID_SIZE]);
bool load_territory(const char* const, const char* const,
                    player_t[GRID_SIZE][GRID_SIZE]);
bool load_bot(const char* const, const char* const, uint8_t* const);
bool load_money(const char* const, const char* const, money_t[PLAYERS_CAPACITY]);
bool load_team(const char* const, char*, uint8_t* const);
bool load_unit(const char* const, const char* const, const model_t,
               struct units* const);
bool load_units(const char* const, const char* const, struct units* const);
bool load_command(struct game* const, const char* const, char* const,
                  grid_t* const);
#endif
bool file_load(struct game* const, const char* const);
#ifdef EXPOSE_FILE_INTERNALS
void save_turn(const player_t, FILE* const);
grid_wide_t calc_row_length(const tile_t[GRID_SIZE]);
grid_wide_t calc_row_count(const tile_t map[GRID_SIZE][GRID_SIZE]);
void save_map(const tile_t[GRID_SIZE][GRID_SIZE], FILE* const);
void save_unit(const struct unit* const unit, FILE* const file);
void save_units(const struct units* const, FILE* const);
void save_territory(const player_t[GRID_SIZE][GRID_SIZE], FILE* const);
void save_monies(const money_t[PLAYERS_CAPACITY], FILE* const);
void save_bots(const uint8_t* const, FILE* const);
void save_teams(const uint8_t* const alliances, FILE* const file);
void save_game(const struct game* const, FILE* const);
#endif
bool file_save(const struct game* const, const char* const);

#endif
