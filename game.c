#include "game.h"
#include "action.h"
#include "bitarray.h"
#include "constants.h"
#include "file.h"
#include "grid.h"
#include "unit_constants.h"
#include <assert.h>
#include <string.h>

void game_initialise(struct game* const game) {
    memset(game, 0, sizeof(struct game));
    grid_clear_territory(game->territory);
    units_initialise(&game->units);
}

bool are_turns_empty(const struct game* const game) {
    if (game->units.size > 0)
        return false;

    for (player_t player = 0; player < PLAYERS_CAPACITY; ++player)
        if (game->golds[player] > 0)
            return false;

    return true;
}

void skip_turns(struct game* const game) {
    for (player_t player = 0; player < PLAYERS_CAPACITY; ++player)
        game->golds[player] += game->incomes[player];
}

void skip_empty_turns(struct game* const game) {
    if (are_turns_empty(game))
        skip_turns(game);
}

bool game_load(struct game* const game, const char* const filename) {
    game_initialise(game);
    const bool error = file_load(game, filename);
    grid_correct_territory(game->territory, game->map);
    grid_compute_incomes(game->territory, game->incomes);
    skip_empty_turns(game);
    return error;
}

bool game_save(struct game* const game, const char* const filename) {
    return file_save(game, filename);
}

void game_deselect(struct game* const game) {
    units_clear_selection(&game->units);
    grid_clear_labels(game);
}

health_t calc_damage(const struct game* const game,
                     const struct unit* const attacker,
                     const struct unit* const attackee) {
    typedef uint32_t damage_t;
    const damage_t defense_max = 10;
    const damage_t attack_max = 100;

    const tile_t tile = game->map[attackee->y][attackee->x];
    const movement_t movement_type = unit_movement_types[attackee->model];
    return ((damage_t)units_damage[attacker->model][attackee->model] *
            (damage_t)attacker->health *
            (damage_t)(defense_max - tile_defense[movement_type][tile])) /
           (attack_max * defense_max);
}

void calc_damage_pair(const struct game* const game,
                      const struct unit* const attacker,
                      const struct unit* const attackee, health_t* const damage,
                      health_t* const counter_damage) {
    struct unit attackee_copy = *attackee;

    *damage = calc_damage(game, attacker, attackee);

    // Apply damage
    if (*damage > attackee->health) {
        *counter_damage = 0;
        return;
    }

    attackee_copy.health -= *damage;

    // Ranged units do not give counter-attacks
    if (models_min_range[attacker->model] || models_min_range[attackee->model])
        *counter_damage = 0;
    else
        *counter_damage = calc_damage(game, &attackee_copy, attacker);
}

void calc_damage_pair_with_health(const struct game* const game,
                                  const struct unit* const attacker,
                                  const struct unit* const attackee,
                                  const health_t attacker_health,
                                  health_t* const damage,
                                  health_t* const counter_damage) {
    struct unit attacker_copy = *attacker;
    attacker_copy.health = attacker_health;
    calc_damage_pair(game, &attacker_copy, attackee, damage, counter_damage);
}

void game_calc_damage(const struct game* const game, health_t* const damage,
                      health_t* const counter_damage) {
    const struct unit* const attacker = units_const_get_selected(&game->units);
    const struct unit* const adjacent =
        units_const_get_at_safe(&game->units, game->prev_x, game->prev_y);
    const struct unit* const attackee =
        units_const_get_at(&game->units, game->x, game->y);

    if (units_is_ranged(attacker->model) || adjacent == NULL ||
        !adjacent->enabled || attacker == adjacent) {
        calc_damage_pair(game, attacker, attackee, damage, counter_damage);
        return;
    }

    const health_t merged_health = units_merge_health(attacker, adjacent);
    calc_damage_pair_with_health(game, attacker, attackee, merged_health,
                                 damage, counter_damage);
}

bool game_is_attackable(const struct game* const game) {
    // The state is attackable iff:
    // 1. A unit is selected
    // 2. Previous selected tile is accessible if direct attack
    // 3. Selected tile is attackable, which implies:
    //     a. Can attack with positive damage
    //     b. Attacker and attackee are in different teams
    //     c. Attackee in attacker's range
    return units_has_selection(&game->units) &&
           (models_min_range[units_const_get_selected(&game->units)->model] ||
            game->labels[game->prev_y][game->prev_x] & ACCESSIBLE_BIT) &&
           game->labels[game->y][game->x] & ATTACKABLE_BIT;
}

bool game_is_buildable(const struct game* const game) {
    // The state is buildable iff:
    // 1. The player owns the selected capturable
    // 2. There is no unit on the tile
    // 3. The capturable has buildable units
    // 4. No unit is selected

    if (game->territory[game->y][game->x] != game->turn)
        return false;

    const tile_t capturable = game->map[game->y][game->x] - TERRIAN_CAPACITY;

    return !units_exists(&game->units, game->x, game->y) &&
           buildable_models[capturable] < buildable_models[capturable + 1] &&
           !units_has_selection(&game->units) &&
           units_is_insertable(&game->units);
}

// A player is alive iff:
// 1. The player has units
// 2. The player has a HQ, implied by a positive income
//    This holds because when a player loses their HQ, income is nullified
bool game_is_alive(const struct game* const game, const player_t player) {
    return units_is_owner(&game->units, player) || game->incomes[player] > 0;
}

bool game_is_bot(const struct game* const game, const player_t player) {
    return bitarray_get(game->bots, player);
}

bool game_is_friendly(const struct game* const game, const player_t player) {
    return bitmatrix_get(game->alliances, game->turn, player);
}

void game_remove_player(struct game* const game, const player_t player) {
    units_delete_player(&game->units, player);
    grid_clear_player_territory(game->map, game->territory, player);
    game->incomes[player] = 0;
}
