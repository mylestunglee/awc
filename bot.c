#include "bot.h"
#include "action.h"
#include "build_units.h"
#include "constants.h"
#include "game.h"
#include "grid.h"
#include "unit_constants.h"
#include <assert.h>
#include <limits.h>
#include <stdlib.h>

const struct unit* find_attackee(struct game* const game,
                                 const struct unit* const attacker) {
    const struct unit* best_attackee = NULL;
    typedef int16_t metric_t;
    metric_t best_metric = SHRT_MIN;

    game->y = 0;
    do {
        game->x = 0;
        do {
            // Attackee is at an attack-labelled tile
            if (!(game->labels[game->y][game->x] & ATTACKABLE_BIT))
                continue;

            health_t damage, counter_damage;
            game_simulate_attack(game, &damage, &counter_damage);

            const struct unit* const attackee =
                units_const_get_at(&game->units, game->x, game->y);
            assert(attackee);
            const metric_t damage_metric =
                (metric_t)damage * (metric_t)models_cost[attackee->model];
            const metric_t counter_damage_metric =
                (metric_t)counter_damage *
                (metric_t)models_cost[attacker->model];

            const metric_t metric = damage_metric - counter_damage_metric;

            if (metric > best_metric) {
                best_metric = metric;
                best_attackee = attackee;
            }
        } while (++game->x);
    } while (++game->y);

    return best_attackee;
}

static void handle_ranged_attack(struct game* const game,
                                 struct unit* const attacker,
                                 const struct unit* const attackee) {
    game->x = attackee->x;
    game->y = attackee->y;
    action_attack(game);
}

static void handle_direct_attack(struct game* const game,
                                 struct unit* const attacker,
                                 const struct unit* const attackee) {

    // Find maximal defense tile around attackee
    energy_t max_energy = 0;
    health_t max_defense = 0;
    uint8_t best_i = 0;

    const grid_t x = attackee->x;
    const grid_t y = attackee->y;
    const grid_t adjacent_x[] = {(grid_t)(x + 1), x, (grid_t)(x - 1), x};
    const grid_t adjacent_y[] = {y, (grid_t)(y - 1), y, (grid_t)(y + 1)};

    // Each tile has four adjacent tiles
    for (uint8_t i = 0; i < 4; ++i) {
        const grid_t i_x = adjacent_x[i];
        const grid_t i_y = adjacent_y[i];

        if (!(game->labels[i_y][i_x] & ACCESSIBLE_BIT))
            continue;

        const model_t model = attacker->model;
        const tile_t tile = game->map[i_y][i_x];
        const health_t defense = tile_defense[model][tile];
        const energy_t energy = game->energies[i_y][i_x];

        // Lexicographical ordering over defence then energy
        if (defense > max_defense ||
            (defense == max_defense && energy > max_energy)) {
            max_defense = defense;
            max_energy = energy;
            best_i = i;
        }
    }

    assert(max_energy > 0);

    // Apply attack
    game->x = x;
    game->y = y;
    game->prev_x = adjacent_x[best_i];
    game->prev_y = adjacent_y[best_i];
    action_attack(game);
}

static void handle_attack(struct game* const game,
                          struct unit* const attacker) {
    const struct unit* const attackee = find_attackee(game, attacker);

    if (attackee == NULL)
        return;

    if (models_min_range[attacker->model])
        handle_ranged_attack(game, attacker, attackee);
    else
        handle_direct_attack(game, attacker, attackee);
}

static void update_max_energy(const struct game* const game, grid_t x, grid_t y,
                              energy_t* const max_energy,
                              grid_t* const update_x, grid_t* const update_y) {

    const energy_t energy = game->energies[y][x];

    if (energy <= *max_energy)
        return;

    if (!(game->labels[y][x] & ACCESSIBLE_BIT))
        return;

    *max_energy = energy;
    *update_x = x;
    *update_y = y;
}

static energy_t find_nearest_capturable(struct game* const game,
                                        grid_t* const nearest_x,
                                        grid_t* const nearest_y) {
    // Maximise remaining energy to find nearest
    energy_t max_energy = 0;
    grid_t y = 0;
    do {
        grid_t x = 0;
        do {
            // Tile is capturable
            if (game->map[y][x] < TERRIAN_CAPACITY)
                continue;

            if (game_is_friendly(game, game->territory[y][x]))
                continue;

            update_max_energy(game, x, y, &max_energy, nearest_x, nearest_y);
        } while (++x);
    } while (++y);

    return max_energy;
}

// Capture nearest enemy capturable
static void handle_capture(struct game* const game, struct unit* const unit) {
    assert(unit->enabled);

    // Unit can capture
    if (unit->model >= UNIT_CAPTURABLE_UPPER_BOUND)
        return;

    grid_t x, y;
    const energy_t found = find_nearest_capturable(game, &x, &y);

    if (found == 0)
        return;

    game->x = x;
    game->y = y;
    bool success = action_move(game);
    assert(success);
}

// Attempt single-turn operation
static void handle_local(struct game* const game, struct unit* const unit) {
    assert(unit->enabled);

    // Populate labels and workspace
    game->x = unit->x;
    game->y = unit->y;

    // Scan for local targets
    grid_explore(game, false);
    handle_attack(game, unit);
    if (!unit->enabled) {
        return;
    }

    handle_capture(game, unit);

    if (!unit->enabled)
        return;

    assert(game->dirty_labels);
    grid_clear_labels(game);
}

static void find_nearest_attackee_target_ranged(
    const struct game* const game, const struct unit* const attacker,
    const struct unit* const attackee, energy_t* const max_energy,
    grid_t* const nearest_x, grid_t* const nearest_y) {

    const model_t model = attacker->model;
    const grid_wide_t max_range = models_max_range[model];

    for (grid_wide_t j = -max_range; j <= max_range; ++j)
        for (grid_wide_t i = -max_range; i <= max_range; ++i) {
            const grid_wide_t distance = abs(i) + abs(j);
            if (models_min_range[model] <= distance && distance <= max_range)
                update_max_energy(game, (grid_wide_t)(attackee->x) + i,
                                  (grid_wide_t)(attackee->y) + j, max_energy,
                                  nearest_x, nearest_y);
        }
}

static void find_nearest_attackee_target_direct(
    const struct game* const game, const struct unit* const attackee,
    energy_t* const max_energy, grid_t* const nearest_x,
    grid_t* const nearest_y) {

    const grid_t x = attackee->x;
    const grid_t y = attackee->y;
    const grid_t adjacent_x[] = {(grid_t)(x + 1), x, (grid_t)(x - 1), x};
    const grid_t adjacent_y[] = {y, (grid_t)(y - 1), y, (grid_t)(y + 1)};

    for (uint8_t i = 0; i < 4; ++i)
        update_max_energy(game, adjacent_x[i], adjacent_y[i], max_energy,
                          nearest_x, nearest_y);
}

static energy_t find_nearest_attackee_target(struct game* const game,
                                             const struct unit* const attacker,
                                             grid_t* const nearest_x,
                                             grid_t* const nearest_y) {

    // Maximise remaining energy to find nearest
    energy_t max_energy = 0;

    for (player_t player = 0; player < PLAYERS_CAPACITY; ++player) {

        if (game_is_friendly(game, player))
            continue;

        const struct unit* attackee =
            units_const_get_first(&game->units, player);
        while (attackee) {
            // Attackee is attackable
            if (units_damage[attacker->model][attackee->model] > 0) {
                // If attacker is ranged
                if (models_min_range[attacker->model])
                    find_nearest_attackee_target_ranged(game, attacker,
                                                        attackee, &max_energy,
                                                        nearest_x, nearest_y);
                else
                    find_nearest_attackee_target_direct(
                        game, attackee, &max_energy, nearest_x, nearest_y);
            }

            attackee = units_const_get_next(&game->units, attackee);
        }
    }

    return max_energy;
}

static bool find_nearest_target(struct game* const game,
                                struct unit* const unit,
                                grid_t* const nearest_x,
                                grid_t* const nearest_y) {

    grid_t attackee_target_x, attackee_target_y;
    const energy_t attackee_target_energy = find_nearest_attackee_target(
        game, unit, &attackee_target_x, &attackee_target_y);

    energy_t capturable_energy = 0;

    if (unit->model < UNIT_CAPTURABLE_UPPER_BOUND) {
        grid_t capturable_x, capturable_y;
        capturable_energy =
            find_nearest_capturable(game, &capturable_x, &capturable_y);

        if (attackee_target_energy > capturable_energy) {
            *nearest_x = attackee_target_x;
            *nearest_y = attackee_target_y;
        } else {
            *nearest_x = capturable_x;
            *nearest_y = capturable_y;
        }
    } else {
        *nearest_x = attackee_target_x;
        *nearest_y = attackee_target_y;
    }

    return attackee_target_energy > 0 || capturable_energy > 0;
}

static void move_towards_target(struct game* const game,
                                struct unit* const unit, const grid_t x,
                                const grid_t y) {

    grid_find_path(game, x, y);

    struct list* const list = &game->list;

    assert(!list_empty(list));

    const energy_t accessible_energy =
        list_back_peek(list).energy - unit_movement_ranges[unit->model];

    while (!list_empty(list) &&
           list_back_peek(list).energy >= accessible_energy) {
        const struct list_node node = list_back_pop(list);
        if (ACCESSIBLE_BIT & game->labels[node.y][node.x]) {
            game->x = node.x;
            game->y = node.y;
        }
    }

    assert(ACCESSIBLE_BIT & game->labels[game->y][game->x]);

    const bool success = action_move(game);
    assert(success);
}

static void handle_nonlocal(struct game* const game, struct unit* const unit) {
    // Number of turns of unit movement to look ahead
    const energy_t look_ahead = 16;

    assert(game->x == unit->x);
    assert(game->y == unit->y);

    // label_attackable_tiles argument=false is unimportant because attack_bit
    // is unread
    grid_explore_recursive(game, false, look_ahead);
    grid_t x, y;
    bool found = find_nearest_target(game, unit, &x, &y);

    if (found)
        move_towards_target(game, unit, x, y);
    else {
        assert(game->dirty_labels);
        grid_clear_labels(game);
    }
}

static void interact_unit(struct game* const game, struct unit* const unit) {
    assert(game->turn == unit->player);

    if (!unit->enabled)
        return;

    // TODO: tidy selection by pointer?
    units_select_at(&game->units, unit->x, unit->y);
    handle_local(game, unit);

    if (unit->enabled)
        handle_nonlocal(game, unit);

    units_clear_selection(&game->units);
}

static void interact_units(struct game* const game) {
    assert(!units_has_selection(&game->units));

    struct units* const units = &game->units;
    struct unit* unit = units_get_first(units, game->turn);
    while (unit) {
        interact_unit(game, unit);
        unit = units_get_next(units, unit);
    }
}

void bot_play(struct game* const game) {
    game_deselect(game);
    interact_units(game);
    build_units(game);
}
