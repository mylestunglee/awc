#include "game.h"
#include "constants.h"
#include "action.h"
#include "bitarray.h"
#include "file.h"
#include "grid.h"
#include <assert.h>
#include <string.h>
#include "unit_constants.h"

#define defense_max (health_wide_t)10
#define attack_max (health_wide_t)100

void game_initialise(struct game* const game) {
    memset(game, 0, sizeof(struct game));
    grid_clear_territory(game->territory);
    units_initialise(&game->units);
}

bool game_load(struct game* const game, const char* const filename) {
    game_initialise(game);
    const bool error = file_load(game, filename);
    grid_correct_territory(game->territory, game->map);
    grid_compute_incomes(game->territory, game->incomes);
    return error;
}

void game_deselect(struct game* const game) {
    units_clear_selection(&game->units);
    grid_clear_labels(game);
}

const struct unit* find_next_unit(const struct game* const game) {
    const struct unit* const curr =
        units_const_get_at(&game->units, game->x, game->y);
    if (curr && curr->player == game->turn) {
        const struct unit* next = curr;

        do {
            next = units_const_get_next_cyclic(&game->units, next);

            if (!next || next == curr)
                return NULL;
        } while (!next->enabled);

        return next;
    } else {
        const struct unit* next =
            units_const_get_first(&game->units, game->turn);

        while (next && !next->enabled)
            next = units_const_get_next(&game->units, next);

        return next;
    }
}

// Hovers the next enabled unit, returns unit was selected
bool game_hover_next_unit(struct game* const game) {
    const struct unit* const unit = find_next_unit(game);
    if (!unit)
        return false;

    assert(unit->enabled);

    game->x = unit->x;
    game->y = unit->y;

    game_deselect(game);

    return true;
}

// Calculate damage when attacker attacks attackee
health_t calc_damage(const struct game* const game,
                     const struct unit* const attacker,
                     const struct unit* const attackee) {

    const tile_t tile = game->map[attackee->y][attackee->x];
    const uint8_t movement_type = unit_movement_types[attackee->model];
    return ((health_wide_t)units_damage[attacker->model][attackee->model] *
            (health_wide_t)attacker->health *
            (health_wide_t)(defense_max - tile_defense[movement_type][tile])) /
           (attack_max * defense_max);
}

// Calculate damage and counter-damage values without performing attack
void game_simulate_attack(const struct game* const game, health_t* const damage,
                          health_t* const counter_damage) {

    const struct unit* const attacker =
        units_const_get_by(&game->units, game->units.selected);
    struct unit attackee = *units_const_get_at(&game->units, game->x, game->y);

    *damage = calc_damage(game, attacker, &attackee);

    // Apply damage
    if (*damage > attackee.health) {
        *counter_damage = 0;
        return;
    }

    attackee.health -= *damage;

    // Ranged units do not give counter-attacks
    if (models_min_range[attacker->model] || models_min_range[attackee.model])
        *counter_damage = 0;
    else
        *counter_damage = calc_damage(game, &attackee, attacker);
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
            game->labels[game->prev_y][game->prev_x] & accessible_bit) &&
           game->labels[game->y][game->x] & attackable_bit;
}

bool game_is_buildable(const struct game* const game) {
    // The state is buildable iff:
    // 1. The player owns the selected capturable
    // 2. There is no unit on the tile
    // 3. The capturable has buildable units
    // 4. No unit is selected

    if (game->territory[game->y][game->x] != game->turn)
        return false;

    const tile_t capturable = game->map[game->y][game->x] - terrian_capacity;

    return !units_const_get_at(&game->units, game->x, game->y) &&
           buildable_models[capturable] < buildable_models[capturable + 1] &&
           !units_has_selection(&game->units);
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
    assert(player != null_player);
    units_delete_player(&game->units, player);
    grid_clear_player_territory(game->map, game->territory, player);
    game->incomes[player] = 0;
}
