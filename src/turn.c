#include "turn.h"
#include "bot.h"
#include "constants.h"
#include "grid.h"
#include "unit_constants.h"
#include "units.h"
#include <assert.h>

void repair_units(struct game* const game) {
    struct unit* unit = units_get_first(&game->units, game->turn);
    while (unit) {
        if (game->territory[unit->y][unit->x] == game->turn &&
            unit->health < HEALTH_MAX) {

            health_t heal = HEAL_RATE;

            // Cap heal at maximum health
            if (unit->health >= HEALTH_MAX - heal)
                heal = HEALTH_MAX - unit->health;

            unit->health += heal;
            game->monies[game->turn] -=
                (model_costs[unit->model] * (money_t)heal) /
                (money_t)HEALTH_MAX;
        }

        unit = units_get_next(&game->units, unit);
    }
}

void start_turn(struct game* const game) {
    units_set_enabled(&game->units, game->turn, true);
    game->monies[game->turn] += game->incomes[game->turn];
    repair_units(game);
}

void end_turn(struct game* const game) {
    units_clear_selection(&game->units);
    grid_clear_labels(game);
    units_set_enabled(&game->units, game->turn, false);
}

void next_alive_turn(struct game* const game) {
    for (player_t i = 0; i < PLAYERS_CAPACITY; ++i) {
        game->turn = (game->turn + 1) % PLAYERS_CAPACITY;
        if (game_is_alive(game, game->turn))
            return;
    }
}

bool exists_alive_non_bot(const struct game* const game) {
    for (player_t player = 0; player < PLAYERS_CAPACITY; ++player)
        if (game_is_alive(game, player) && !game_is_bot(game, player))
            return true;

    return false;
}

void turn_next(struct game* const game) {
    do {
        if (game_is_bot(game, game->turn))
            bot_play(game);

        end_turn(game);
        next_alive_turn(game);
        start_turn(game);
    } while (exists_alive_non_bot(game) && game_is_bot(game, game->turn));
}
