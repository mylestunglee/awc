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
bool load_gold(const char* const, const char* const, gold_t[PLAYERS_CAPACITY]);
bool load_team(const char* const, char*, uint8_t* const);
bool load_unit(const char* const, const char* const, const model_t,
               struct units* const);
bool load_units(const char* const, const char* const, struct units* const);
bool load_command(struct game* const, const char* const, char* const,
                  grid_t* const);
#endif
bool file_load(struct game* const, const char* const);
#ifdef EXPOSE_FILE_INTERNALS
grid_wide_t calc_row_length(const tile_t[GRID_SIZE]);
void file_save_map(const tile_t map[GRID_SIZE][GRID_SIZE], FILE* const file);
#endif
bool file_save(const struct game* const, const char* const);

#endif
