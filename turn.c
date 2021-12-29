#include "turn.h"
#include "bot.h"
#include "grid.h"
#include "units.h"
#include <assert.h>

void repair_units(struct game* const game) {
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

void start_turn(struct game* const game) {
    units_set_enabled(&game->units, game->turn, true);
    game->golds[game->turn] += gold_scale * game->incomes[game->turn];
    repair_units(game);
}

void end_turn(struct game* const game) {
    units_clear_selection(&game->units);
    grid_clear_labels(game);
    units_set_enabled(&game->units, game->turn, false);
}

void next_alive_turn(struct game* const game) {
    for (player_t i = 0; i < players_capacity; ++i) {
        game->turn = (game->turn + 1) % players_capacity;
        if (game_is_alive(game, game->turn))
            return;
    }
    assert(false);
}

bool exists_alive_non_bot(const struct game* const game) {
    for (player_t player = 0; player < players_capacity; ++player)
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

    game_hover_next_unit(game);
}