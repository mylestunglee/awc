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

    return sscanf(params, PLAYER_FORMAT, turn) == 1;
}

bool load_map(const char* const command, const char* const params,
              grid_t* const y, tile_t map[GRID_SIZE][GRID_SIZE]) {
    if (strcmp(command, "map"))
        return false;

    grid_t x = 0;

    // Read symbols left to right
    while (params[x] != '\0') {
        // Find tile index for symbol
        for (tile_t tile = 0; tile < TILE_CAPACITY; ++tile)
            if (params[x] == tile_symbols[tile]) {
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

    if (player >= PLAYERS_CAPACITY)
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

    if (player >= PLAYERS_CAPACITY)
        return false;

    bitarray_set(bots, player);

    return true;
}

bool load_money(const char* const command, const char* const params,
                money_t monies[PLAYERS_CAPACITY]) {
    if (strcmp(command, "money"))
        return false;

    player_t player;
    money_t money;

    if (sscanf(params, PLAYER_FORMAT MONEY_FORMAT, &player, &money) != 2)
        return false;

    if (player >= PLAYERS_CAPACITY)
        return false;

    monies[player] = money;

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

        if (player >= PLAYERS_CAPACITY)
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
               const model_t model, struct units* const units,
               struct list* const list) {
    if (strcmp(command, model_names[model]))
        return false;

    player_t player;
    grid_t x, y;
    health_t health = HEALTH_MAX;
    char enabled_buffer[9] = "";
    capture_progress_t capture_progress = 0;

    if (sscanf(params,
               PLAYER_FORMAT GRID_FORMAT GRID_FORMAT HEALTH_FORMAT
                   ENABLED_FORMAT CAPTURE_COMPLETION_FORMAT,
               &player, &x, &y, &health, enabled_buffer, &capture_progress) < 3)
        return false;

    if (player >= PLAYERS_CAPACITY)
        return false;

    if (health > HEALTH_MAX)
        return false;

    if (!units_is_insertable(units))
        return false;

    bool enabled = false;
    if (enabled_buffer[0] == '\0') {
        struct list_node node = {.x = x, .y = y};
        list_insert(list, &node);
    } else if (!strcmp(enabled_buffer, "enabled"))
        enabled = true;
    else if (!strcmp(enabled_buffer, "disabled"))
        enabled = false;
    else
        return false;

    const struct unit unit = {.x = x,
                              .y = y,
                              .player = player,
                              .model = model,
                              .health = health,
                              .capture_progress = capture_progress,
                              .enabled = enabled};

    units_insert(units, &unit);

    return true;
}

bool load_units(const char* const command, const char* const params,
                struct units* const units, struct list* const list) {
    for (model_t model = 0; model < MODEL_CAPACITY; ++model)
        if (load_unit(command, params, model, units, list))
            return true;

    return false;
}

bool load_command(struct game* const game, const char* const command,
                  char* const params) {
    return load_turn(command, params, &game->turn) ||
           load_map(command, params, &game->y, game->map) ||
           load_territory(command, params, game->territory) ||
           load_bot(command, params, game->bots) ||
           load_money(command, params, game->monies) ||
           load_team(command, params, game->alliances) ||
           load_units(command, params, &game->units, &game->list);
}

void resolve_optional_unit_enabled(struct game* const game) {
    while (!list_empty(&game->list)) {
        const struct list_node node = list_front_pop(&game->list);
        struct unit* const unit = units_get_at(&game->units, node.x, node.y);
        unit->enabled = game->turn == unit->player;
    }
}

bool file_load(struct game* const game, const char* const filename) {
    assert(game->y == 0);
    assert(list_empty(&game->list));

    FILE* const file = fopen(filename, "r");

    if (!file)
        return true;

    const char* const delim = " ";
    const uint16_t buffer_size = 4096;
    char line[buffer_size];
    bool error = false;

    while (fgets(line, buffer_size, file)) {
        // Remove trailing new-line character
        line[strcspn(line, "\n")] = '\0';

        char* params;
        char* command = __strtok_r(line, delim, &params);

        if (command == NULL)
            continue;
        else if (load_command(game, command, params))
            continue;
        else
            // Parse remaining lines
            error = true;
    }

    game->y = 0;
    resolve_optional_unit_enabled(game);

    return fclose(file) < 0 || error;
}

void save_turn(const player_t turn, FILE* const file) {
    if (turn != 0)
        fprintf(file, "turn " PLAYER_FORMAT "\n", turn);
}

grid_wide_t calc_row_length(const tile_t row[GRID_SIZE]) {
    for (grid_wide_t x = GRID_SIZE - 1; x >= 0; --x)
        if (row[x] != TILE_VOID)
            return x + 1;

    return 0;
}

grid_wide_t calc_row_count(const tile_t map[GRID_SIZE][GRID_SIZE]) {
    for (grid_wide_t y = GRID_SIZE - 1; y >= 0; --y)
        if (calc_row_length(map[y]) > 0)
            return y + 1;

    return 0;
}

void save_map(const tile_t map[GRID_SIZE][GRID_SIZE], FILE* const file) {
    grid_wide_t row_count = calc_row_count(map);

    for (grid_wide_t y = 0; y < row_count; y++) {
        const grid_wide_t row_length = calc_row_length(map[y]);

        fprintf(file, "map");

        if (row_length > 0) {
            fprintf(file, " ");

            for (grid_wide_t x = 0; x < row_length; ++x)
                fprintf(file, "%c", tile_symbols[map[y][x]]);
        }

        fprintf(file, "\n");
    }
}

void save_unit(const struct unit* const unit, const player_t turn,
               FILE* const file) {
    if (unit->capture_progress == 0) {
        if (unit->enabled == (turn == unit->player)) {
            if (unit->health == HEALTH_MAX)
                fprintf(file,
                        MODEL_NAME_FORMAT " " PLAYER_FORMAT " " GRID_FORMAT
                                          " " GRID_FORMAT "\n",
                        model_names[unit->model], unit->player, unit->x,
                        unit->y);
            else
                fprintf(file,
                        MODEL_NAME_FORMAT " " PLAYER_FORMAT " " GRID_FORMAT
                                          " " GRID_FORMAT " " HEALTH_FORMAT
                                          "\n",
                        model_names[unit->model], unit->player, unit->x,
                        unit->y, unit->health);
        } else
            fprintf(file,
                    MODEL_NAME_FORMAT " " PLAYER_FORMAT " " GRID_FORMAT
                                      " " GRID_FORMAT " " HEALTH_FORMAT
                                      " " ENABLED_FORMAT "\n",
                    model_names[unit->model], unit->player, unit->x, unit->y,
                    unit->health, unit->enabled ? "enabled" : "disabled");
    } else
        fprintf(file,
                MODEL_NAME_FORMAT " " PLAYER_FORMAT " " GRID_FORMAT
                                  " " GRID_FORMAT " " HEALTH_FORMAT
                                  " " ENABLED_FORMAT
                                  " " CAPTURE_COMPLETION_FORMAT "\n",
                model_names[unit->model], unit->player, unit->x, unit->y,
                unit->health, unit->enabled ? "enabled" : "disabled",
                unit->capture_progress);
}

void save_units(const struct units* const units, const player_t turn,
                FILE* const file) {
    for (player_t player = 0; player < PLAYERS_CAPACITY; ++player) {
        const struct unit* unit = units_const_get_first(units, player);
        while (unit) {
            save_unit(unit, turn, file);
            unit = units_const_get_next(units, unit);
        }
    }
}

void save_territory(const player_t territory[GRID_SIZE][GRID_SIZE],
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

void save_monies(const money_t monies[PLAYERS_CAPACITY], FILE* const file) {
    for (player_t player = 0; player < PLAYERS_CAPACITY; ++player)
        if (monies[player])
            fprintf(file, "money " PLAYER_FORMAT " " MONEY_FORMAT "\n", player,
                    monies[player]);
}

void save_bots(const uint8_t* const bots, FILE* const file) {
    for (player_t player = 0; player < PLAYERS_CAPACITY; ++player)
        if (bitarray_get(bots, player))
            fprintf(file, "bot " PLAYER_FORMAT "\n", player);
}

void save_teams(const uint8_t* const alliances, FILE* const file) {
    uint8_t copy[BITMATRIX_SIZE(PLAYERS_CAPACITY)];
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

void save_game(const struct game* const game, FILE* const file) {
    save_turn(game->turn, file);
    save_map(game->map, file);
    save_units(&game->units, game->turn, file);
    save_territory(game->territory, file);
    save_monies(game->monies, file);
    save_bots(game->bots, file);
    save_teams(game->alliances, file);
}

bool file_save(const struct game* const game, const char* const filename) {
    FILE* const file = fopen(filename, "w");

    if (!file)
        return true;

    save_game(game, file);

    return fclose(file) < 0;
}
