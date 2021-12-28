#include "action.h"
#include "grid.h"
#include "turn.h"
#include <assert.h>

// Occurs when unit captures enemy capturable
static void action_capture(struct game* const game) {
    const player_t loser = game->territory[game->y][game->x];

    // Cannot recapture friendly capturable
    assert(loser != game->turn);

    // If the enemy loses their HQ
    if (game->map[game->y][game->x] == tile_hq)
        game_remove_player(game, loser);
    else if (loser != null_player)
        --game->incomes[loser];

    game->territory[game->y][game->x] = game->turn;
    ++game->incomes[game->turn];
}

// refactor to if capturable
bool is_capture_progressable(const struct game* const game) {
    const struct unit* const unit = units_const_get_selected(&game->units);

    assert(unit->player == game->turn);
    assert(unit->x == game->x);
    assert(unit->y == game->y);

    // The moved unit can capture iff:
    // 1. The unit is a infantry or a mech
    // 2. The tile is capturable
    // 3. The tile is owned by an enemy
    if (unit->model >= unit_capturable_upper_bound)
        return false;

    if (game->map[game->y][game->x] < terrian_capacity)
        return false;

    if (game_is_friendly(game, game->territory[game->y][game->x]))
        return false;

    return true;
}

static health_t merge_health(const health_t source, const health_t target) {
    health_wide_t merged_health = (health_wide_t)source + (health_wide_t)target;
    if (merged_health > (health_wide_t)health_max)
        merged_health = health_max;
    return merged_health;
}

// returns actionable health after move
// can modifies which unit is selected
// does not modify enabled
health_t action_move_selected(struct game* const game, const grid_t x,
                              const grid_t y) {
    struct unit* const source = units_get_selected(&game->units);
    assert(source->enabled);
    const struct unit* const target = units_get_at(&game->units, x, y);

    health_t result = source->health;
    if (source != target && target) {
        assert(units_mergable(source, target));
        source->health = merge_health(source->health, target->health);
        if (target->enabled)
            result = source->health;
        units_delete_at(&game->units, x, y);
    }
    units_move_selection(&game->units, x, y);
    units_disable_selection(&game->units);
    return result;
}

// simulate attack with attacker with artifically lower health
static void simulate_restricted_attack(struct game* const game,
                                       const health_t attacker_health,
                                       health_t* const damage,
                                       health_t* const counter_damage) {

    struct unit* const attacker = units_get_selected(&game->units);
    const health_t health = attacker->health;
    assert(attacker_health <= health);
    attacker->health = attacker_health;
    game_simulate_attack(game, damage, counter_damage);
    attacker->health = health;
}

void action_attack(struct game* const game) {
    struct unit* const attacker = units_get_selected(&game->units);
    struct unit* const attackee = units_get_at(&game->units, game->x, game->y);
    assert(attacker);
    assert(attacker->enabled);
    assert(game->labels[game->y][game->x] & attackable_bit);
    assert(game->dirty_labels);
    grid_clear_labels(game);

    // If unit is direct, move to attack
    const bool ranged = models_min_range[attacker->model];
    health_t actionable_health = attacker->health;
    if (ranged) {
        units_delete_selected(&game->units);
    } else {
        actionable_health =
            action_move_selected(game, game->prev_x, game->prev_y);
    }

    // Compute damage
    health_t damage, counter_damage;
    simulate_restricted_attack(game, actionable_health, &damage,
                               &counter_damage);

    // Apply damage
    if (damage >= attackee->health) {
        units_delete_at(&game->units, game->x, game->y);
        units_clear_selection(&game->units);
        return;
    }

    attackee->health -= damage;

    // Ranged units do not receive counter-attacks
    if (ranged) {
        units_clear_selection(&game->units);
        return;
    }

    // Apply counter damage
    if (counter_damage >= attacker->health) {
        units_delete_selected(&game->units);
    } else {
        attacker->health -= counter_damage;
        units_clear_selection(&game->units);
    }
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
                              .enabled = false,
                              .capture_progress = 0};
    const bool error = units_insert(&game->units, &unit);

    return error;
}

bool action_move(struct game* const game) {
    const bool selected = units_has_selection(&game->units);
    if (selected && game->labels[game->y][game->x] & accessible_bit) {
        assert(game->dirty_labels);
        const health_t capture_progress =
            action_move_selected(game, game->x, game->y);
        if (is_capture_progressable(game) &&
            units_update_capture_progress(&game->units, capture_progress))
            action_capture(game);
        game_deselect(game);
        return true;
    }
    return false;
}

bool action_self_destruct_selection(struct game* const game) {
    if (!units_has_selection(&game->units))
        return false;

    units_delete(&game->units, game->units.selected);
    assert(game->dirty_labels);
    game_deselect(game);
    return true;
}

bool at_least_two_alive_players(const struct game* const game) {
    player_t alive_players = 0;
    for (player_t player = 0; player < players_capacity; ++player)
        if (game_is_alive(game, player))
            ++alive_players;
    return alive_players >= 2;
}

bool action_surrender(struct game* const game) {
    if (!at_least_two_alive_players(game))
        return false;

    game_remove_player(game, game->turn);
    turn_next(game);
    return true;
}

bool action_select(struct game* const game) {
    const struct unit* unit =
        units_const_get_at(&game->units, game->x, game->y);
    const bool selected = units_has_selection(&game->units);

    if (!selected && unit && unit->enabled) {
        grid_clear_labels(game);
        grid_explore(game, false);
        units_select_at(&game->units, game->x, game->y);
        return true;
    }

    return false;
}

bool action_highlight(struct game* const game) {
    const bool highlightable = units_exists(&game->units, game->x, game->y) &&
                               !units_has_selection(&game->units);

    if (highlightable)
        grid_explore(game, true);

    return highlightable;
}
