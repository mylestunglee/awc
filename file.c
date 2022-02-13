#include "file.h"
#include "bitarray.h"
#include "constants.h"
#include "format_constants.h"
#include "units.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

static void file_load_map(tile_t map[grid_size][grid_size],
                          const char* const tokens, const grid_t y) {
    char map_str[grid_size] = {0};
    if (sscanf(tokens, row_format, map_str) != 1)
        return;

    grid_t x = 0;

    // Read symbols left to right
    while (map_str[x] != '\0') {
        // Find tile index for symbol
        for (tile_t tile = 0; tile < tile_capacity; ++tile) {
            if (map_str[x] == tile_symbols[tile]) {
                map[y][x] = tile;
                break;
            }
        }
        ++x;
    }
}

static void file_load_territory(player_t territory[grid_size][grid_size],
                                const char* const tokens) {
    grid_t x, y;
    player_t player;

    if (sscanf(tokens, player_format grid_format grid_format, &player, &x,
               &y) != 3)
        return;

    if (player < players_capacity)
        territory[y][x] = player;
}

static void file_load_bot(uint8_t* const bots, const char* const tokens) {
    player_t player;
    if (sscanf(tokens, player_format, &player) != 1)
        return;

    if (player < players_capacity)
        bitarray_set(bots, player);
}

static void file_load_unit(struct units* const units, const char* const tokens,
                           const model_t model) {
    grid_t x, y;
    player_t player;
    health_t health;
    char enabled[8];
    if (sscanf(
            tokens,
            player_format grid_format grid_format health_format enabled_format,
            &player, &x, &y, &health, enabled) != 5)
        return;

    if (player < players_capacity) {
        const struct unit unit = {.health = health,
                                  .model = model,
                                  .player = player,
                                  .x = x,
                                  .y = y,
                                  .enabled = !strcmp(enabled, "enabled")};
        units_insert(units, &unit);
    }
}

static void file_load_gold(gold_t golds[players_capacity],
                           const char* const tokens) {
    player_t player;
    gold_t gold;

    if (sscanf(tokens, player_format gold_format, &player, &gold) != 2)
        return;

    if (player < players_capacity)
        golds[player] = gold;
}

static void file_load_team(uint8_t* const alliances, char* tokens) {
    player_t team[players_capacity];
    player_t size = 0;

    const char* const delim = " ";

    // Parse tokens into integer array
    char* token = __strtok_r(NULL, delim, &tokens);
    while (token) {
        if (sscanf(token, player_format, &team[size]) != 1)
            continue;

        ++size;

        token = __strtok_r(NULL, delim, &tokens);
    }

    // Convert integer array into relation
    for (player_t p = 0; p < size; ++p)
        for (player_t q = p + 1; q < size; ++q) {
            bitmatrix_set(alliances, team[p], team[q]);
        }
}

bool file_load(struct game* const game, const char* const filename) {
    FILE* const file = fopen(filename, "r");

    if (!file)
        return true;

    const char* const delim = " ";
    const uint16_t buffer_size = 4096;
    char line[buffer_size];
    grid_t y = 0;

    while (fgets(line, buffer_size, file)) {
        // Remove trailing new-line character
        line[strcspn(line, "\n")] = '\0';

        char* tokens;
        char* key = __strtok_r(line, delim, &tokens);

        if (key == NULL)
            continue;
        else if (!strcmp(key, "turn"))
            sscanf(tokens, turn_format, &game->turn);
        else if (!strcmp(key, "map")) {
            file_load_map(game->map, tokens, y);
            ++y;
        } else if (!strcmp(key, "territory"))
            file_load_territory(game->territory, tokens);
        else if (!strcmp(key, "gold"))
            file_load_gold(game->golds, tokens);
        else if (!strcmp(key, "bot"))
            file_load_bot(game->bots, tokens);
        else if (!strcmp(key, "team"))
            file_load_team(game->alliances, tokens);
        else
            for (model_t model = 0; model < model_capacity; ++model)
                if (!strcmp(key, model_names[model])) {
                    file_load_unit(&game->units, tokens, model);
                    break;
                }
    }

    return fclose(file) < 0;
}

static grid_wide_t file_row_length(const struct game* const game,
                                   const grid_t y) {
    for (grid_wide_t x = grid_size - 1; x >= 0; --x) {
        if (game->map[y][x]) {
            return x;
        }
    }
    return -1;
}

static void file_save_map(const struct game* const game, FILE* const file) {
    grid_t y = 0;
    do {
        const grid_wide_t length = file_row_length(game, y);

        if (length < 0)
            continue;

        fprintf(file, "map ");

        for (grid_wide_t x = 0; x <= length; ++x) {
            fprintf(file, "%c", tile_symbols[game->map[y][x]]);
        }
        fprintf(file, "\n");
    } while (++y);
}

static void file_save_units(const struct units* const units, FILE* const file) {
    for (player_t player = 0; player < players_capacity; ++player) {
        const struct unit* unit = units_const_get_first(units, player);
        while (unit) {
            fprintf(file,
                    model_name_format " " player_format " " grid_format
                                      " " grid_format " " health_format " %s\n",
                    model_names[unit->model], unit->player, unit->x, unit->y,
                    unit->health, unit->enabled ? "enabled" : "disabled");
            unit = units_const_get_next(units, unit);
        }
    }
}

static void file_save_territory(const player_t territory[grid_size][grid_size],
                                FILE* const file) {
    grid_t y = 0;
    do {
        grid_t x = 0;
        do {
            if (territory[y][x] != null_player)
                fprintf(file,
                        "territory " player_format " " grid_format
                        " " grid_format "\n",
                        territory[y][x], x, y);
        } while (++x);
    } while (++y);
}

static void file_save_golds(const gold_t golds[players_capacity],
                            FILE* const file) {
    for (player_t player = 0; player < players_capacity; ++player)
        if (golds[player])
            fprintf(file, "gold " player_format " " gold_format "\n", player,
                    golds[player]);
}

static void file_save_bots(const uint8_t* const bots, FILE* const file) {
    for (player_t player = 0; player < players_capacity; ++player)
        if (bitarray_get(bots, player))
            fprintf(file, "bot " player_format "\n", player);
}

static void file_save_teams(const uint8_t* const alliances, FILE* const file) {
    uint8_t copy[sizeof(((struct game*)NULL)->alliances)];
    memcpy(copy, alliances, sizeof(copy));

    // For each possible member and following members
    for (player_t p = 0; p < players_capacity; ++p)
        for (player_t s = 0; s < players_capacity; ++s) {
            // Only initalise first team member
            player_t team[players_capacity];
            team[0] = p;
            player_t size = 1;

            // Attempt to grow team for any player expect p
            for (player_t q = 0; q < players_capacity; ++q) {
                if (p == q)
                    continue;

                // q is a member iff every member is friends with q
                bool growable = true;

                for (player_t r = 0; r < size; ++r)
                    if (!bitmatrix_get(copy, q, team[r])) {
                        growable = false;
                        break;
                    }

                // If q is a member, then q is in the team
                if (growable) {
                    // Make q a member
                    team[size] = q;
                    ++size;

                    // Clean excess alliances
                    for (player_t r = 0; r < size; ++r)
                        bitmatrix_unset(copy, q, team[r]);
                }
            }

            // If the team has not grown then future passes of p will not grow
            if (size <= 1)
                break;

            // Print team
            fprintf(file, "team");
            for (player_t r = 0; r < size; ++r)
                fprintf(file, " " player_format, team[r]);
            fprintf(file, "\n");
        }
}

bool file_save(const struct game* const game, const char* const filename) {
    FILE* const file = fopen(filename, "w");

    if (!file)
        return true;

    fprintf(file, "turn " turn_format "\n", game->turn);
    file_save_map(game, file);
    file_save_units(&game->units, file);
    file_save_territory(game->territory, file);
    file_save_golds(game->golds, file);
    file_save_bots(game->bots, file);
    file_save_teams(game->alliances, file);

    return fclose(file) < 0;
}
