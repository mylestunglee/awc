#include "action.h"
#include "bitarray.h"
#include "grid.h"
#include <assert.h>

void action_remove_player(struct game* const game, const player_t player) {
    assert(player != null_player);
    units_delete_player(&game->units, player);
    grid_clear_player_territory(game->map, game->territory, player);
    game->incomes[player] = 0;
}

// Occurs when unit captures enemy capturable
static void action_capture(struct game* const game) {
    const player_t loser = game->territory[game->y][game->x];

    // Cannot recapture friendly capturable
    assert(loser != game->turn);

    // If the enemy loses their HQ
    if (game->map[game->y][game->x] == tile_hq)
        action_remove_player(game, loser);
    else if (loser != null_player)
        --game->incomes[loser];

    game->territory[game->y][game->x] = game->turn;
    ++game->incomes[game->turn];
}

void action_handle_capture(struct game* const game) {
    const struct unit* const unit = units_const_get_selected(&game->units);

    assert(unit->player == game->turn);
    assert(unit->x == game->x);
    assert(unit->y == game->y);

    // The moved unit can capture iff:
    // 1. The unit is a infantry or a mech
    // 2. The tile is capturable
    // 3. The tile is owned by an enemy
    if (unit->model >= unit_capturable_upper_bound)
        return;

    if (game->map[game->y][game->x] < terrian_capacity)
        return;

    if (bitmatrix_get(game->alliances, game->territory[game->y][game->x],
                      unit->player))
        return;

    action_capture(game);
}

void action_attack(struct game* const game) {
    struct unit* const attacker = units_get_selected(&game->units);
    struct unit* const attackee = units_get_at(&game->units, game->x, game->y);
    assert(attacker->enabled);

    // If unit is direct, move to attack
    const bool ranged = models_min_range[attacker->model];
    if (!ranged)
        units_move_selection(&game->units, game->prev_x, game->prev_y);

    // Compute damage
    health_t damage, counter_damage;
    game_simulate_attack(game, &damage, &counter_damage);

    // Apply damage
    if (damage >= attackee->health) {
        units_delete_at(&game->units, game->x, game->y);
        return;
    }

    attackee->health -= damage;

    // Ranged units do not receive counter-attacks
    if (ranged)
        return;

    // Apply counter damage
    if (counter_damage >= attacker->health) {
        units_delete_selected(&game->units);
        return;
    }

    attacker->health -= counter_damage;
    attacker->enabled = false;
}

// Build unit at (game->x, game->y), returns true iff build is successful
bool action_build(struct game* const game, const model_t model) {
    const gold_t cost = gold_scale * models_cost[model];

    assert(game->map[game->y][game->x] >= terrian_capacity);
    const tile_t capturable = game->map[game->y][game->x] - terrian_capacity;

    if (model < buildable_models[capturable])
        return true;

    if (model >= buildable_models[capturable + 1])
        return true;

    if (game->golds[game->turn] < cost)
        return true;

    game->golds[game->turn] -= cost;

    // Error may occur when units is full
    const struct unit unit = {.health = health_max,
                              .model = model,
                              .player = game->turn,
                              .x = game->x,
                              .y = game->y,
                              .enabled = false};
    const bool error = units_insert(&game->units, &unit);

    return error;
}

void action_move(struct game* const game) {
    grid_clear_uint8(game->labels);
    units_move_selection(&game->units, game->x, game->y);
    action_handle_capture(game);
    units_disable_selection(&game->units);
}

bool action_self_destruct_selection(struct game* const game) {
    if (!units_has_selection(&game->units))
        return false;

    units_delete(&game->units, game->units.selected);
    units_clear_selection(&game->units);
    grid_clear_uint8(game->labels);
    // if bot calls this function, then clear energies
    return true;
}
