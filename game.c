#include "game.h"
#include "action.h"
#include "bitarray.h"
#include "bot.h"
#include "console.h"
#include "file.h"
#include "graphics.h"
#include "grid.h"
#include <stdio.h>

void game_initialise(struct game* const game) {
    // TODO: fix order
    game->x = 0;
    game->y = 0;
    grid_clear_uint8(game->map);
    grid_clear_uint8(game->labels);
    grid_clear_energy(game->energies);
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
}

static bool calc_build_enabled(const struct game* const game) {
    // The state is build enabled iff:
    // 1. The player owns the selected capturable
    // 2. There is no unit on the tile
    // 3. The capturable has buildable units
    // 4. No unit is selected

    if (game->territory[game->y][game->x] != game->turn)
        return false;

    const tile_t capturable = game->map[game->y][game->x] - terrian_capacity;

    return game->units.grid[game->y][game->x] == null_unit &&
           buildable_models[capturable] < buildable_models[capturable + 1] &&
           game->units.selected == null_unit;
}

static void move_cursor_to_interactable(struct game* const game) {
    // Attempt to select a unit
    const struct unit* const unit =
        units_const_get_first(&game->units, game->turn);

    if (unit) {
        game->x = unit->x;
        game->y = unit->y;
        return;
    }

    // Attempt to select a buildable
    game->x = 0;
    game->y = 0;
    do {
        do {
            if (calc_build_enabled(game))
                return;

            if (game->map[game->y][game->x] == tile_hq &&
                game->territory[game->y][game->x] == game->turn)
                return;
        } while (++game->x);
    } while (++game->y);

    // game->x, game->y undefined when no interactable
}

static void game_postload(struct game* const game) {
    grid_correct(game->territory, game->map);
    grid_compute_incomes(game->territory, game->incomes);
    move_cursor_to_interactable(game);
}

bool game_load(struct game* const game, const char* const filename) {
    game_initialise(game);
    const bool error = file_load(game, filename);
    game_postload(game);
    return error;
}

static bool parse_panning(struct game* const game, const char input) {
    switch (input) {
    case 'w': {
        game->prev_x = game->x;
        game->prev_y = game->y;
        --game->y;
        return true;
    }
    case 'a': {
        game->prev_x = game->x;
        game->prev_y = game->y;
        --game->x;
        return true;
    }
    case 's': {
        game->prev_x = game->x;
        game->prev_y = game->y;
        ++game->y;
        return true;
    }
    case 'd': {
        game->prev_x = game->x;
        game->prev_y = game->y;
        ++game->x;
        return true;
    }
    }
    return false;
}

// Returns the unit index of the cyclic next unit of the same player
// If no unit can be suggested, null_player is returned
static unit_t suggest_next_unit(struct game* const game, const unit_t hint) {
    const struct units* units = &game->units;
    unit_t next_unit = hint;
    if (hint == null_unit)
        next_unit = units->firsts[game->turn];

    while (true) {
        if (units->data[next_unit].enabled)
            return next_unit;
        next_unit = units->nexts[next_unit];
        if (next_unit == null_unit) {
            if (hint == null_unit)
                return null_unit;
            else
                next_unit = units->firsts[game->turn];
        }
        if (next_unit == hint)
            return null_unit;
    }
}

static void reset_selection(struct game* const game) {
    game->units.selected = null_unit;
    grid_clear_uint8(game->labels);
}

// Selects the next enabled unit of the current turn, returns true iff unit was
// selected
static bool select_next_unit(struct game* const game) {
    const struct units* units = &game->units;
    const unit_t cursor_unit = units->grid[game->y][game->x];
    unit_t next_unit_index;
    if (cursor_unit != null_unit &&
        units->data[cursor_unit].player == game->turn &&
        units->data[cursor_unit].enabled)
        next_unit_index = suggest_next_unit(game, units->nexts[cursor_unit]);
    else
        next_unit_index = suggest_next_unit(game, null_unit);

    if (next_unit_index == null_unit || next_unit_index == game->units.selected)
        return false;

    const struct unit* const next_unit = &game->units.data[next_unit_index];
    game->x = next_unit->x;
    game->y = next_unit->y;
    reset_selection(game);

    return true;
}

static bool parse_select_next_unit(struct game* const game, const char input) {
    return input == 'm' && select_next_unit(game);
}

static bool parse_self_distruct_unit(struct game* const game,
                                     const char input) {
    if (input != 'k')
        return false;

    if (game->units.selected == null_unit)
        return false;

    units_delete(&game->units, game->units.selected);
    reset_selection(game);
    return true;
}

// Build try to build a unit, assume build enabled
static bool parse_build(struct game* const game, const char input) {
    assert(game->map[game->y][game->x] >= terrian_capacity);

    const tile_t capturable = game->map[game->y][game->x] - terrian_capacity;
    const model_t value = input - '1';

    if (value >=
        buildable_models[capturable + 1] - buildable_models[capturable])
        return false;

    return !action_build(game, value + buildable_models[capturable]);
}

static bool parse_file(struct game* const game, const char input) {
    bool error;

    switch (input) {
    case '1': {
        error = game_load(game, "state1.txt");
        break;
    }
    case '2': {
        error = game_load(game, "state2.txt");
        break;
    }
    case '3': {
        error = game_load(game, "state3.txt");
        break;
    }
    case '4': {
        error = game_load(game, "state4.txt");
        break;
    }
    case '5': {
        error = game_load(game, "state5.txt");
        break;
    }
    case '6': {
        error = file_save(game, "state1.txt");
        break;
    }
    case '7': {
        error = file_save(game, "state2.txt");
        break;
    }
    case '8': {
        error = file_save(game, "state3.txt");
        break;
    }
    case '9': {
        error = file_save(game, "state4.txt");
        break;
    }
    case '0': {
        error = file_save(game, "state5.txt");
        break;
    }
    default:
        return false;
    }

    if (error)
        printf("IO error");

    return true;
}

static bool calc_attack_enabled(const struct game* const game) {
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

static void handle_unit_selection(struct game* const game) {
    const unit_t unit = game->units.grid[game->y][game->x];

    // Select unit iff:
    // 1. A unit is not already selected
    // 2. The unit is enabled
    if (game->units.selected == null_unit && unit != null_unit) {
        const bool select = game->units.data[unit].enabled;

        // Remove highlighting of disabled units
        if (select)
            grid_clear_uint8(game->labels);

        // Allow highlighting of disabled units
        grid_explore(game, !select, true);
        grid_clear_energy(game->energies);

        if (select)
            game->units.selected = unit;
    } else {
        // Move to accessible tile when cursor is not over unit
        if (game->units.selected != null_unit &&
            game->labels[game->y][game->x] & accessible_bit) {

            units_move(&game->units, game->units.selected, game->x, game->y);
            action_handle_capture(game);
            game->units.data[game->units.selected].enabled = false;
        }

        reset_selection(game);
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
    const struct unit* const attackee =
        units_const_get_at(&game->units, game->x, game->y);

    *damage = calc_damage(game, attacker, attackee);

    // Apply damage
    if (*damage > attackee->health) {
        *counter_damage = 0;
        return;
    }

    // Ranged units do not give counter-attacks
    if (models_min_range[attackee->model])
        *counter_damage = 0;
    else
        *counter_damage = calc_damage(game, attackee, attacker);
}

static void game_attack(struct game* const game) {
    action_attack(game);
    reset_selection(game);
}

static void repair_units(struct game* const game) {
    struct unit* unit = units_get_first(&game->units, game->turn);
    while (unit) {
        if (game->territory[unit->y][unit->x] == game->turn &&
            unit->health < health_max) {

            health_t heal = heal_rate;

            // Cap heal at maximum health
            if (unit->health >= health_max - heal)
                heal = health_max - unit->health;

            unit->health += heal;
            game->golds[game->turn] -=
                (models_cost[unit->model] * gold_scale * (gold_t)heal) /
                (gold_t)health_max;
        }

        unit = units_get_next(&game->units, unit);
    }
}

// A player is alive iff:
// 1. The player has units
// 2. The player has a HQ, implied by a positive income
//    This holds because when a player loses their HQ, income is nullified
static bool is_alive(const struct game* const game, const player_t player) {
    return game->units.firsts[player] != null_unit || game->incomes[player] > 0;
}

static void end_turn(struct game* const game) {
    reset_selection(game);
    units_set_enabled(&game->units, game->turn, false);
}

static void start_turn(struct game* const game) {
    units_set_enabled(&game->units, game->turn, true);
    game->golds[game->turn] += gold_scale * game->incomes[game->turn];
    repair_units(game);
}

static bool is_bot(const struct game* const game, const player_t player) {
    return bitarray_get(game->bots, player);
}

static void next_alive_turn(struct game* const game) {
    for (player_t i = 0; i < players_capacity; ++i) {
        game->turn = (game->turn + 1) % players_capacity;
        if (is_alive(game, game->turn))
            return;
    }
    assert(false);
}

static bool exists_alive_non_bot(const struct game* const game) {
    for (player_t player = 0; player < players_capacity; ++player)
        if (is_alive(game, player) && !is_bot(game, player))
            return true;

    return false;
}

static void next_turn(struct game* const game) {
    do {
        if (is_bot(game, game->turn))
            bot_play(game);

        end_turn(game);
        next_alive_turn(game);
        start_turn(game);
    } while (exists_alive_non_bot(game) && is_bot(game, game->turn));

    move_cursor_to_interactable(game);
}

static bool at_least_two_alive_players(const struct game* const game) {
    player_t alive_players = 0;
    for (player_t player = 0; player < players_capacity; ++player)
        if (is_alive(game, player))
            ++alive_players;
    return alive_players >= 2;
}

static bool parse_surrender(struct game* const game, const char input) {
    if (input != 'K')
        return false;

    if (!at_least_two_alive_players(game))
        return false;

    action_remove_player(game, game->turn);
    next_turn(game);
    return true;
}

static void print_normal_text(const struct game* const game) {
    printf("turn=%hhu x=%hhu y=%hhu tile=%s territory=%hhu label=%u gold=%u",
           game->turn, game->x, game->y,
           tile_names[game->map[game->y][game->x]],
           game->territory[game->y][game->x], game->labels[game->y][game->x],
           game->golds[game->turn]);
}

static void print_attack_text(const struct game* const game) {
    health_t damage, counter_damage;
    game_simulate_attack(game, &damage, &counter_damage);
    const health_wide_t percent = 100;
    printf("Damage: %u%% Counter-damage: %u%%", (damage * percent) / health_max,
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
}

static void print_text(const struct game* const game, const bool attack_enabled,
                       const bool build_enabled) {

    if (attack_enabled)
        print_attack_text(game);
    else if (build_enabled)
        print_build_text(game);
    else
        print_normal_text(game);
}

void game_loop(struct game* const game) {
    do {
        const bool attack_enabled = calc_attack_enabled(game);
        const bool build_enabled = calc_build_enabled(game);
        assert(!(attack_enabled && build_enabled));

        render(game, attack_enabled, build_enabled);
        print_text(game, attack_enabled, build_enabled);

        const char input = getch();

        if (input == 'q')
            break;

        if (input == 'n') {
            next_turn(game);
            continue;
        }

        if (parse_panning(game, input))
            continue;

        if (parse_select_next_unit(game, input))
            continue;

        if (parse_self_distruct_unit(game, input))
            continue;

        if (parse_surrender(game, input))
            continue;

        if (build_enabled && parse_build(game, input))
            continue;

        if (parse_file(game, input))
            continue;

        if (input == ' ') {
            if (attack_enabled)
                game_attack(game);
            else
                handle_unit_selection(game);
        }
    } while (true);

    // New line after print text
    printf("\n");
}
