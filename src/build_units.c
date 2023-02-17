#include "build_units.h"
#include "action.h"
#include "bap_solver.h"
#include "definitions.h"
#include "unit_constants.h"
#include <assert.h>

void accumulate_distribution(const struct game* const game,
                             const player_t player,
                             health_wide_t distribution[MODEL_CAPACITY]) {
    const struct units* const units = &game->units;
    const struct unit* unit = units_const_get_first(units, player);
    while (unit) {
        distribution[unit->model] += unit->health;
        unit = units_const_get_next(units, unit);
    }
}

void inputs_initialise_distributions(const struct game* const game,
                                     struct bap_inputs* const inputs) {
    for (player_t player = 0; player < PLAYERS_CAPACITY; ++player) {
        if (game_is_friendly(game, player))
            accumulate_distribution(game, player,
                                    inputs->friendly_distribution);
        else
            accumulate_distribution(game, player, inputs->enemy_distribution);
    }
}

void inputs_initialise_capturables(struct game* const game,
                                   struct bap_inputs* const inputs) {
    game->y = 0;
    do {
        game->x = 0;
        do {
            if (!game_is_buildable(game))
                continue;

            const tile_t capturable =
                game->map[game->y][game->x] - TERRIAN_CAPACITY;
            ++inputs->capturables[capturable];
        } while (++game->x);
    } while (++game->y);
}

void inputs_initialise(struct game* const game,
                       struct bap_inputs* const inputs) {
    inputs_initialise_distributions(game, inputs);
    inputs_initialise_capturables(game, inputs);
    inputs->budget = game->golds[game->turn];
}

void realise_allocations(struct game* const game,
                         grid_wide_t allocations[MODEL_CAPACITY]) {
    game->y = 0;
    do {
        game->x = 0;
        do {
            if (!game_is_buildable(game))
                continue;

            const tile_t capturable =
                game->map[game->y][game->x] - TERRIAN_CAPACITY;

            for (model_t model = capturable_buildable_models[capturable];
                 model < capturable_buildable_models[capturable + 1]; ++model) {
                if (allocations[model] == 0)
                    continue;

                const bool error = action_build(game, model);
                assert(!error);

                --allocations[model];
                break;
            }
        } while (++game->x);
    } while (++game->y);
}

void build_units(struct game* const game) {
    assert(!units_has_selection(&game->units));
    struct bap_inputs inputs = {0};
    inputs_initialise(game, &inputs);
    grid_wide_t allocations[MODEL_CAPACITY] = {0};
    bap_solve(&inputs, allocations, &game->list);
    realise_allocations(game, allocations);
}
