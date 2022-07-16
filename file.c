#include "file.h"
#include "bitarray.h"
#include "constants.h"
#include "format_constants.h"
#include "units.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

bool load_turn(const char* const command, const char* const params,
               player_t* const turn) {
    if (strcmp(command, "turn"))
        return false;

    return sscanf(params, TURN_FORMAT, turn) == 1;
}

bool load_map(const char* const command, const char* const params,
              grid_t* const y, tile_t map[GRID_SIZE][GRID_SIZE]) {
    if (strcmp(command, "map"))
        return false;

    char map_str[GRID_SIZE] = {0};
    assert(sscanf(params, ROW_FORMAT, map_str) == 1);

    grid_t x = 0;

    // Read symbols left to right
    while (map_str[x] != '\0') {
        // Find tile index for symbol
        for (tile_t tile = 0; tile < TILE_CAPACITY; ++tile)
            if (map_str[x] == tile_symbols[tile]) {
                map[*y][x] = tile;
                break;
            }
        ++x;
    }

    ++*y;

    return true;
}

bool load_territory(const char* const command, const char* const params,
                    player_t territory[GRID_SIZE][GRID_SIZE]) {
    if (strcmp(command, "territory"))
        return false;

    grid_t x, y;
    player_t player;

    if (sscanf(params, PLAYER_FORMAT GRID_FORMAT GRID_FORMAT, &player, &x,
               &y) != 3)
        return false;

    if (player >= NULL_PLAYER)
        return false;

    territory[y][x] = player;

    return true;
}

bool load_bot(const char* const command, const char* const params,
              uint8_t* const bots) {
    if (strcmp(command, "bot"))
        return false;

    player_t player;

    if (sscanf(params, PLAYER_FORMAT, &player) != 1)
        return false;

    if (player >= NULL_PLAYER)
        return false;

    bitarray_set(bots, player);

    return true;
}

bool load_gold(const char* const command, const char* const params,
               gold_t golds[PLAYERS_CAPACITY]) {
    if (strcmp(command, "gold"))
        return false;

    player_t player;
    gold_t gold;

    if (sscanf(params, PLAYER_FORMAT GOLD_FORMAT, &player, &gold) != 2)
        return false;

    if (player >= NULL_PLAYER)
        return false;

    golds[player] = gold;

    return true;
}

bool load_team(const char* const command, char* params,
               uint8_t* const alliances) {
    if (strcmp(command, "team"))
        return false;

    player_t team[PLAYERS_CAPACITY];
    player_t size = 0;

    const char* const delim = " ";

    // Parse tokens into integer array
    char* token = __strtok_r(NULL, delim, &params);
    while (token) {
        player_t player;

        if (sscanf(token, PLAYER_FORMAT, &player) != 1)
            return false;

        if (player >= NULL_PLAYER)
            return false;

        team[size] = player;
        ++size;

        token = __strtok_r(NULL, delim, &params);
    }

    if (size == 0)
        return false;

    // Convert integer array into relation
    for (player_t p = 0; p < size; ++p)
        for (player_t q = p + 1; q < size; ++q)
            bitmatrix_set(alliances, team[p], team[q]);

    return true;
}

bool load_unit(const char* const command, const char* const params,
               const model_t model, struct units* const units) {
    if (strcmp(command, model_names[model]))
        return false;

    grid_t x, y;
    player_t player;
    health_t health;
    char enabled_buffer[9];

    if (sscanf(
            params,
            PLAYER_FORMAT GRID_FORMAT GRID_FORMAT HEALTH_FORMAT ENABLED_FORMAT,
            &player, &x, &y, &health, enabled_buffer) != 5)
        return false;

    if (player >= NULL_PLAYER)
        return false;

    bool enabled;
    if (!strcmp(enabled_buffer, "enabled"))
        enabled = true;
    else if (!strcmp(enabled_buffer, "disabled"))
        enabled = false;
    else
        return false;

    const struct unit unit = {.health = health,
                              .model = model,
                              .player = player,
                              .x = x,
                              .y = y,
                              .enabled = enabled};
    units_insert(units, &unit);

    return true;
}

bool load_units(const char* const command, const char* const params,
                struct units* const units) {
    for (model_t model = 0; model < MODEL_CAPACITY; ++model)
        if (load_unit(command, params, model, units))
            return true;

    return false;
}

bool file_load(struct game* const game, const char* const filename) {
    FILE* const file = fopen(filename, "r");

    if (!file)
        return true;

    const char* const delim = " ";
    const uint16_t buffer_size = 4096;
    char line[buffer_size];
    grid_t y = 0;
    bool error = false;

    while (fgets(line, buffer_size, file)) {
        // Remove trailing new-line character
        line[strcspn(line, "\n")] = '\0';

        char* params;
        char* command = __strtok_r(line, delim, &params);

        if (command == NULL)
            continue;
        else if (load_turn(command, params, &game->turn) ||
                 load_map(command, params, &y, game->map) ||
                 load_territory(command, params, game->territory) ||
                 load_bot(command, params, game->bots) ||
                 load_gold(command, params, game->golds) ||
                 load_team(command, params, game->alliances) ||
                 load_units(command, params, &game->units))
            continue;
        else
            // Parse remaining lines
            error = true;
    }

    return fclose(file) < 0 || error;
}

static grid_wide_t file_row_length(const struct game* const game,
                                   const grid_t y) {
    for (grid_wide_t x = GRID_SIZE - 1; x >= 0; --x) {
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
    for (player_t player = 0; player < PLAYERS_CAPACITY; ++player) {
        const struct unit* unit = units_const_get_first(units, player);
        while (unit) {
            fprintf(file,
                    MODEL_NAME_FORMAT " " PLAYER_FORMAT " " GRID_FORMAT
                                      " " GRID_FORMAT " " HEALTH_FORMAT " %s\n",
                    model_names[unit->model], unit->player, unit->x, unit->y,
                    unit->health, unit->enabled ? "enabled" : "disabled");
            unit = units_const_get_next(units, unit);
        }
    }
}

static void file_save_territory(const player_t territory[GRID_SIZE][GRID_SIZE],
                                FILE* const file) {
    grid_t y = 0;
    do {
        grid_t x = 0;
        do {
            if (territory[y][x] != NULL_PLAYER)
                fprintf(file,
                        "territory " PLAYER_FORMAT " " GRID_FORMAT
                        " " GRID_FORMAT "\n",
                        territory[y][x], x, y);
        } while (++x);
    } while (++y);
}

static void file_save_golds(const gold_t golds[PLAYERS_CAPACITY],
                            FILE* const file) {
    for (player_t player = 0; player < PLAYERS_CAPACITY; ++player)
        if (golds[player])
            fprintf(file, "gold " PLAYER_FORMAT " " GOLD_FORMAT "\n", player,
                    golds[player]);
}

static void file_save_bots(const uint8_t* const bots, FILE* const file) {
    for (player_t player = 0; player < PLAYERS_CAPACITY; ++player)
        if (bitarray_get(bots, player))
            fprintf(file, "bot " PLAYER_FORMAT "\n", player);
}

static void file_save_teams(const uint8_t* const alliances, FILE* const file) {
    uint8_t copy[sizeof(((struct game*)NULL)->alliances)];
    memcpy(copy, alliances, sizeof(copy));

    // For each possible member and following members
    for (player_t p = 0; p < PLAYERS_CAPACITY; ++p)
        for (player_t s = 0; s < PLAYERS_CAPACITY; ++s) {
            // Only initalise first team member
            player_t team[PLAYERS_CAPACITY];
            team[0] = p;
            player_t size = 1;

            // Attempt to grow team for any player expect p
            for (player_t q = 0; q < PLAYERS_CAPACITY; ++q) {
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
                fprintf(file, " " PLAYER_FORMAT, team[r]);
            fprintf(file, "\n");
        }
}

bool file_save(const struct game* const game, const char* const filename) {
    FILE* const file = fopen(filename, "w");

    if (!file)
        return true;

    fprintf(file, "turn " TURN_FORMAT "\n", game->turn);
    file_save_map(game, file);
    file_save_units(&game->units, file);
    file_save_territory(game->territory, file);
    file_save_golds(game->golds, file);
    file_save_bots(game->bots, file);
    file_save_teams(game->alliances, file);

    return fclose(file) < 0;
}
