#include "game.h"
#include "action.h"
#include "bitarray.h"
#include "file.h"
#include "grid.h"
#include <assert.h>
#include <stdio.h>

void game_initialise(struct game* const game) {
    // TODO: fix order
    game->x = 0;
    game->y = 0;
    grid_clear_uint8(game->map);
    grid_clear_uint8(game->labels);
    grid_clear_territory(game->territory);
    units_initialise(&game->units);
    list_initialise(&game->list);
    game->turn = 0;

    for (player_t player = 0; player < players_capacity; ++player) {
        game->golds[player] = 0;
        game->incomes[player] = 0;
    }

    bitarray_clear(game->bots, sizeof(game->bots));
    bitarray_clear(game->alliances, sizeof(game->alliances));
    game->dirty_labels = false;
}

bool game_build_enabled(const struct game* const game) {
    // The state is build enabled iff:
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

static void game_postload(struct game* const game) {
    grid_correct(game->territory, game->map);
    grid_compute_incomes(game->territory, game->incomes);
}

bool game_load(struct game* const game, const char* const filename) {
    game_initialise(game);
    const bool error = file_load(game, filename);
    game_postload(game);
    return error;
}

static void game_reset_selection(struct game* const game) {
    units_clear_selection(&game->units);
    grid_clear_labels(game);
}

// Selects the next enabled unit of the current turn, returns true iff unit was
// selected
// TODO: implement when a hovering over a unit
bool game_select_next_unit(struct game* const game) {
    const struct unit* const unit =
        units_const_get_first(&game->units, game->turn);
    if (!unit)
        return false;

    game->x = unit->x;
    game->y = unit->y;
    game_reset_selection(game);

    return true;
}

bool game_attack_enabled(const struct game* const game) {
    // The state is attack enabled iff:
    // 1. A unit is selected
    // 2. Previous selected tile is accessible if direct attack
    // 3. Selected tile is attackable, which implies:
    //     a. Selected unit can attack with positive damage
    //     b. Attacker and attackee are in different teams
    return game->units.selected != null_unit &&
           (models_min_range[units_const_get_by(&game->units,
                                                game->units.selected)
                                 ->model] ||
            game->labels[game->prev_y][game->prev_x] & accessible_bit) &&
           game->labels[game->y][game->x] & attackable_bit;
}

static void select_unit(struct game* const game) {
    grid_explore(game, false, true);
    units_select_at(&game->units, game->x, game->y);
}

static void highlight_unit(struct game* const game) {
    grid_explore(game, true, true);
}

void game_handle_unit_selection(struct game* const game) {
    const struct unit* unit =
        units_const_get_at(&game->units, game->x, game->y);
    const bool selected = units_has_selection(&game->units);

    // Select unit iff:
    // 1. A unit is not already selected
    // 2. The unit is enabled
    if (!selected && unit) {
        if (unit->enabled) {
            grid_clear_labels(game);
            select_unit(game);
        } else
            highlight_unit(game);
    } else {
        // Move to accessible tile when cursor is not over unit
        if (selected && game->labels[game->y][game->x] & accessible_bit) {
            action_move(game);
        } else
            game_reset_selection(game);
    }
}

// Calculate damage when attacker attacks attackee
static health_t calc_damage(const struct game* const game,
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
    if (models_min_range[attackee.model])
        *counter_damage = 0;
    else
        *counter_damage = calc_damage(game, &attackee, attacker);
}

// A player is alive iff:
// 1. The player has units
// 2. The player has a HQ, implied by a positive income
//    This holds because when a player loses their HQ, income is nullified
bool game_is_alive(const struct game* const game, const player_t player) {
    return game->units.firsts[player] != null_unit || game->incomes[player] > 0;
}

bool game_is_bot(const struct game* const game, const player_t player) {
    return bitarray_get(game->bots, player);
}

static void print_normal_text(const struct game* const game) {
    printf("turn=%hhu x=%hhu y=%hhu tile=%s territory=%hhu label=%u gold=%u\n",
           game->turn, game->x, game->y,
           tile_names[game->map[game->y][game->x]],
           game->territory[game->y][game->x], game->labels[game->y][game->x],
           game->golds[game->turn]);
}

static void print_attack_text(const struct game* const game) {
    health_t damage, counter_damage;
    game_simulate_attack(game, &damage, &counter_damage);
    const health_wide_t percent = 100;
    printf("Damage: %u%% Counter-damage: %u%%\n",
           (damage * percent) / health_max,
           (counter_damage * percent) / health_max);
}

static void print_build_text(const struct game* const game) {
    const tile_t tile = game->map[game->y][game->x];
    assert(tile >= terrian_capacity);
    const tile_t capturable = tile - terrian_capacity;

    printf("in build mode:");
    for (model_t model = buildable_models[capturable];
         model < buildable_models[capturable + 1]; ++model) {
        printf("(" model_format ") %s ", model + 1, model_names[model]);
    }
    printf("\n");
}

void game_print_text(const struct game* const game, const bool attack_enabled,
                     const bool build_enabled) {

    if (attack_enabled)
        print_attack_text(game);
    else if (build_enabled)
        print_build_text(game);
    else
        print_normal_text(game);
}

void game_remove_player(struct game* const game, const player_t player) {
    assert(player != null_player);
    units_delete_player(&game->units, player);
    grid_clear_player_territory(game->map, game->territory, player);
    game->incomes[player] = 0;
}
