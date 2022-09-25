#include "action.h"
#include "constants.h"
#include "grid.h"
#include "turn.h"
#include "unit_constants.h"
#include <assert.h>
#include <stddef.h>

health_t move_selected_unit(struct game* const game, const grid_t x,
                            const grid_t y) {
    struct unit* const source = units_get_selected(&game->units);
    assert(source->enabled);
    const struct unit* const target = units_get_at(&game->units, x, y);

    health_t result = source->health;
    if (source != target && target) {
        source->health = units_merge_health(source, target);
        if (target->enabled)
            result = source->health;
        units_delete_at(&game->units, x, y);
    }

    units_move_selection(&game->units, x, y);
    units_disable_selection(&game->units);
    return result;
}

bool action_attack(struct game* const game) {
    if (!game_is_attackable(game))
        return true;

    struct unit* const attacker = units_get_selected(&game->units);
    struct unit* const attackee = units_get_at(&game->units, game->x, game->y);
    assert(attacker);
    assert(attacker->enabled);
    assert(game->labels[game->y][game->x] & ATTACKABLE_BIT);
    assert(game->dirty_labels);
    grid_clear_labels(game);

    health_t damage, counter_damage;
    game_calc_damage(game, &damage, &counter_damage);

    if (units_direct(attacker->model))
        (void)move_selected_unit(game, game->prev_x, game->prev_y);

    // Apply damage
    if (damage >= attackee->health) {
        units_delete_at(&game->units, game->x, game->y);
        units_clear_selection(&game->units);
        assert(counter_damage == 0);
        return false;
    }

    attackee->health -= damage;

    // Apply counter damage
    if (counter_damage >= attacker->health) {
        units_delete_selected(&game->units);
    } else {
        attacker->health -= counter_damage;
        units_clear_selection(&game->units);
    }

    return false;
}

// Build unit at (game->x, game->y), returns true iff build is successful
bool action_build(struct game* const game, const model_t model) {
    if (!game_is_buildable(game))
        return true;

    const gold_t cost = models_cost[model];
    const tile_t capturable = game->map[game->y][game->x] - TERRIAN_CAPACITY;

    if (model < buildable_models[capturable] ||
        model >= buildable_models[capturable + 1] ||
        game->golds[game->turn] < cost)
        return true;

    game->golds[game->turn] -= cost;

    // Error may occur when units is full
    const struct unit unit = {.x = game->x,
                              .y = game->y,
                              .player = game->turn,
                              .model = model,
                              .health = HEALTH_MAX,
                              .enabled = false};
    const bool error = units_insert(&game->units, &unit);

    return error;
}

bool can_selected_unit_capture(const struct game* const game) {
    const struct unit* const unit = units_const_get_selected(&game->units);

    assert(unit->player == game->turn);
    assert(unit->x == game->x);
    assert(unit->y == game->y);

    // The moved unit can capture iff:
    // 1. The unit is a infantry or a mech
    // 2. The tile is capturable
    // 3. The tile is owned by an enemy
    return unit->model < UNIT_CAPTURABLE_UPPER_BOUND &&
           game->map[game->y][game->x] >= TERRIAN_CAPACITY &&
           !game_is_friendly(game, game->territory[game->y][game->x]);
}

// Occurs when unit captures enemy capturable
void action_capture(struct game* const game) {
    const player_t loser = game->territory[game->y][game->x];

    // Cannot recapture friendly capturable
    assert(loser != game->turn);

    // If the enemy loses their HQ
    if (game->map[game->y][game->x] == TILE_HQ)
        game_remove_player(game, loser);
    else if (loser != NULL_PLAYER)
        game->incomes[loser] -= GOLD_SCALE;

    game->territory[game->y][game->x] = game->turn;
    game->incomes[game->turn] += GOLD_SCALE;
}

bool action_move(struct game* const game) {
    const bool selected = units_has_selection(&game->units);
    if (selected && game->labels[game->y][game->x] & ACCESSIBLE_BIT) {
        assert(game->dirty_labels);
        const health_t capture_progress =
            move_selected_unit(game, game->x, game->y);
        if (can_selected_unit_capture(game) &&
            units_update_capture_progress(&game->units, capture_progress))
            action_capture(game);
        game_deselect(game);
        return false;
    }
    return true;
}

bool action_self_destruct(struct game* const game) {
    if (!units_has_selection(&game->units))
        return true;

    units_delete_selected(&game->units);
    assert(game->dirty_labels);
    game_deselect(game);
    return false;
}

bool at_least_two_alive_players(const struct game* const game) {
    player_t alive_players = 0;
    for (player_t player = 0; player < PLAYERS_CAPACITY; ++player)
        if (game_is_alive(game, player))
            ++alive_players;
    return alive_players >= 2;
}

bool action_surrender(struct game* const game) {
    if (!at_least_two_alive_players(game))
        return true;

    game_remove_player(game, game->turn);
    turn_next(game);
    return false;
}

bool action_select(struct game* const game) {
    const struct unit* unit =
        units_const_get_at(&game->units, game->x, game->y);
    const bool selected = units_has_selection(&game->units);

    if (!selected && unit && unit->enabled) {
        grid_clear_labels(game);
        grid_explore(game, false);
        units_select_at(&game->units, game->x, game->y);
        return false;
    }

    return true;
}

bool action_highlight(struct game* const game) {
    const bool highlightable = units_exists(&game->units, game->x, game->y) &&
                               !units_has_selection(&game->units);

    if (highlightable)
        grid_explore(game, true);

    return !highlightable;
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
bool action_hover_next_unit(struct game* const game) {
    const struct unit* const unit = find_next_unit(game);
    if (!unit)
        return true;

    assert(unit->enabled);

    game->x = unit->x;
    game->y = unit->y;

    game_deselect(game);

    return false;
}
