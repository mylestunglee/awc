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

void simulate_defended_attack(struct game* const game, health_t* const damage,
                              health_t* const counter_damage) {
    const struct unit* const attacker = units_const_get_selected(&game->units);

    if (units_ranged(attacker->model)) {
        game_calc_damage(game, damage, counter_damage);
        return;
    }

    const tile_t original_tile = game->map[attacker->y][attacker->x];
    const grid_t x = game->x;
    const grid_t y = game->y;
    const grid_t adjacent_x[] = {(grid_t)(x + 1), x, (grid_t)(x - 1), x};
    const grid_t adjacent_y[] = {y, (grid_t)(y - 1), y, (grid_t)(y + 1)};
    const movement_t movement = unit_movement_types[attacker->model];
    health_t best_defense = 0;

    for (uint8_t i = 0; i < 4; ++i) {
        const grid_t x_i = adjacent_x[i];
        const grid_t y_i = adjacent_y[i];
        const tile_t tile = game->map[y_i][x_i];
        const health_t defense = tile_defense[movement][tile];
        if (game->labels[y_i][x_i] & ACCESSIBLE_BIT &&
            defense >= best_defense) {
            best_defense = defense;
            game->map[attacker->y][attacker->x] = tile;
            game->prev_x = x_i;
            game->prev_x = y_i;
        }
    }

    game_calc_damage(game, damage, counter_damage);
    game->map[attacker->y][attacker->x] = original_tile;
}

const struct unit* find_attackee(struct game* const game,
                                 const model_t attacker_model) {
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
            simulate_defended_attack(game, &damage, &counter_damage);

            const struct unit* const attackee =
                units_const_get_at(&game->units, game->x, game->y);
            assert(attackee);
            const metric_t damage_metric =
                (metric_t)damage * (metric_t)models_cost[attackee->model];
            const metric_t counter_damage_metric =
                (metric_t)counter_damage *
                (metric_t)models_cost[attacker_model];

            const metric_t metric = damage_metric - counter_damage_metric;

            if (metric > best_metric) {
                best_metric = metric;
                best_attackee = attackee;
            }
        } while (++game->x);
    } while (++game->y);

    return best_attackee;
}

void set_prev_position(struct game* const game, const model_t attacker_model,
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
        const grid_t x_i = adjacent_x[i];
        const grid_t y_i = adjacent_y[i];

        if (!(game->labels[y_i][x_i] & ACCESSIBLE_BIT))
            continue;

        const tile_t tile = game->map[y_i][x_i];
        const health_t defense = tile_defense[attacker_model][tile];
        const energy_t energy = game->energies[y_i][x_i];

        // Lexicographical ordering of defense over energy
        if (defense > max_defense ||
            (defense == max_defense && energy > max_energy)) {
            max_defense = defense;
            max_energy = energy;
            best_i = i;
        }
    }

    assert(max_energy > 0);

    game->prev_x = adjacent_x[best_i];
    game->prev_y = adjacent_y[best_i];
}

void prepare_attack(struct game* const game, const model_t attacker_model,
                    const struct unit* const attackee) {
    game->x = attackee->x;
    game->y = attackee->y;

    if (!models_min_range[attacker_model])
        set_prev_position(game, attacker_model, attackee);
}

void handle_attack(struct game* const game, const model_t attacker_model) {
    const struct unit* const attackee = find_attackee(game, attacker_model);

    if (attackee == NULL)
        return;

    prepare_attack(game, attacker_model, attackee);
    action_attack(game);
}

energy_t update_max_energy(const struct game* const game,
                           const energy_t max_energy, grid_t* const updated_x,
                           grid_t* const updated_y) {
    const grid_t x = game->x;
    const grid_t y = game->y;
    const energy_t energy = game->energies[y][x];

    if (energy > max_energy && game->labels[y][x] & ACCESSIBLE_BIT) {
        *updated_x = x;
        *updated_y = y;
        return energy;
    }

    return max_energy;
}

energy_t find_nearest_capturable(struct game* const game) {
    grid_t nearest_x = 0;
    grid_t nearest_y = 0;
    // Maximise remaining energy to find nearest
    energy_t max_energy = 0;
    game->y = 0;
    do {
        game->x = 0;
        do {
            if (game->map[game->y][game->x] < TERRIAN_CAPACITY)
                continue;

            if (game_is_friendly(game, game->territory[game->y][game->x]))
                continue;

            max_energy =
                update_max_energy(game, max_energy, &nearest_x, &nearest_y);
        } while (++game->x);
    } while (++game->y);

    game->x = nearest_x;
    game->y = nearest_y;

    return max_energy;
}

void handle_capture(struct game* const game, const model_t model) {
    if (model >= UNIT_CAPTURABLE_UPPER_BOUND)
        return;

    const bool found = find_nearest_capturable(game) > 0;

    if (!found)
        return;

    bool success = action_move(game);
    assert(success);
}

void handle_local(struct game* const game, const struct unit* const unit) {
    assert(unit->enabled);
    const model_t model = unit->model;

    game->x = unit->x;
    game->y = unit->y;
    grid_explore(game, false);

    handle_attack(game, model);

    if (!unit->enabled)
        return;

    handle_capture(game, model);

    if (!unit->enabled)
        return;

    grid_clear_labels(game);
}

energy_t find_nearest_attackable_attackee_ranged(
    struct game* const game, const model_t attacker_model,
    const struct unit* const attackee, energy_t max_energy,
    grid_t* const nearest_x, grid_t* const nearest_y) {

    const grid_wide_t max_range = models_max_range[attacker_model];

    for (grid_wide_t j = -max_range; j <= max_range; ++j)
        for (grid_wide_t i = -max_range; i <= max_range; ++i) {
            const grid_wide_t distance = abs(i) + abs(j);
            if (models_min_range[attacker_model] <= distance &&
                distance <= max_range) {
                game->x = (grid_wide_t)(attackee->x) + i;
                game->y = (grid_wide_t)(attackee->y) + j;

                max_energy =
                    update_max_energy(game, max_energy, nearest_x, nearest_y);
            }
        }

    return max_energy;
}

energy_t find_nearest_attackable_attackee_direct(
    struct game* const game, const struct unit* const attackee,
    energy_t max_energy, grid_t* const nearest_x, grid_t* const nearest_y) {

    const grid_t x = attackee->x;
    const grid_t y = attackee->y;
    const grid_t adjacent_x[] = {(grid_t)(x + 1), x, (grid_t)(x - 1), x};
    const grid_t adjacent_y[] = {y, (grid_t)(y - 1), y, (grid_t)(y + 1)};

    for (uint8_t i = 0; i < 4; ++i) {
        game->x = adjacent_x[i];
        game->y = adjacent_y[i];
        max_energy = update_max_energy(game, max_energy, nearest_x, nearest_y);
    }

    return max_energy;
}

energy_t find_nearest_attackable_attackee(struct game* const game,
                                          const model_t attacker_model,
                                          const struct unit* const attackee,
                                          energy_t max_energy,
                                          grid_t* const nearest_x,
                                          grid_t* const nearest_y) {
    if (models_min_range[attacker_model])
        return find_nearest_attackable_attackee_ranged(
            game, attacker_model, attackee, max_energy, nearest_x, nearest_y);
    else
        return find_nearest_attackable_attackee_direct(
            game, attackee, max_energy, nearest_x, nearest_y);
}

energy_t find_nearest_attackable(struct game* const game,
                                 const model_t attacker_model,
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
            if (units_damage[attacker_model][attackee->model] > 0)
                max_energy = find_nearest_attackable_attackee(
                    game, attacker_model, attackee, max_energy, nearest_x,
                    nearest_y);

            attackee = units_const_get_next(&game->units, attackee);
        }
    }

    return max_energy;
}

bool find_nearest_target(struct game* const game, const model_t attacker_model,
                         grid_t* const nearest_x, grid_t* const nearest_y) {

    grid_t attackee_target_x, attackee_target_y;
    const energy_t attackee_target_energy = find_nearest_attackable(
        game, attacker_model, &attackee_target_x, &attackee_target_y);

    energy_t capturable_energy = 0;

    if (attacker_model < UNIT_CAPTURABLE_UPPER_BOUND) {
        capturable_energy = find_nearest_capturable(game);
        const grid_t capturable_x = game->x;
        const grid_t capturable_y = game->y;

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

void move_towards_target(struct game* const game, const model_t model,
                         const grid_t x, const grid_t y) {

    grid_find_path(game, x, y);

    struct list* const list = &game->list;

    assert(!list_empty(list));

    // Restrict accessible energy to one turn
    const energy_t accessible_energy =
        list_back_peek(list).energy - unit_movement_ranges[model];

    while (!list_empty(list) &&
           list_back_peek(list).energy >= accessible_energy) {
        const struct list_node node = list_back_pop(list);
        if (ACCESSIBLE_BIT & game->labels[node.y][node.x]) {
            game->x = node.x;
            game->y = node.y;
        }
    }

    assert(ACCESSIBLE_BIT & game->labels[game->y][game->x]);
}

void handle_nonlocal(struct game* const game, struct unit* const unit) {
    // Number of turns of unit movement to look ahead
    const energy_t look_ahead = 16;

    game->x = unit->x;
    game->y = unit->y;

    // label_attackable_tiles argument=false is unimportant because attack_bit
    // is unread
    grid_explore_recursive(game, false, look_ahead);
    grid_t target_x, target_y;
    bool found = find_nearest_target(game, unit->model, &target_x, &target_y);

    if (found) {
        move_towards_target(game, unit->model, target_x, target_y);
        const bool success = action_move(game);
        assert(success);
    } else {
        assert(game->dirty_labels);
        grid_clear_labels(game);
    }
}

void interact_unit(struct game* const game, struct unit* const unit) {
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
