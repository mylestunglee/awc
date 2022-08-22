#include "parse.h"
#include "action.h"
#include "constants.h"
#include "file.h"
#include "turn.h"
#include "unit_constants.h"

bool parse_build(struct game* const game, const model_t offset) {
    if (game->map[game->y][game->x] < TERRIAN_CAPACITY)
        return false;

    const tile_t capturable = game->map[game->y][game->x] - TERRIAN_CAPACITY;
    const model_t model = offset + buildable_models[capturable];

    return action_build(game, model);
}

bool parse_1(struct game* const game) {
    return parse_build(game, 0) && game_load(game, "state1.txt");
}

bool parse_command(struct game* const game, const char input) {
    switch (input) {
    case KEY_NEXT_TURN: {
        turn_next(game);
        action_hover_next_unit(game);
        return false;
    }
    case KEY_PAN_UP: {
        game->prev_x = game->x;
        game->prev_y = game->y;
        --game->y;
        return false;
    }
    case KEY_PAN_LFFT: {
        game->prev_x = game->x;
        game->prev_y = game->y;
        --game->x;
        return false;
    }
    case KEY_PAN_DOWN: {
        game->prev_x = game->x;
        game->prev_y = game->y;
        ++game->y;
        return false;
    }
    case KEY_PAN_RIGHT: {
        game->prev_x = game->x;
        game->prev_y = game->y;
        ++game->x;
        return false;
    }
    case KEY_HOVER_NEXT_UNIT: {
        return action_hover_next_unit(game);
    }
    case KEY_SELF_DESTRUCT: {
        return action_self_destruct(game);
    }
    case KEY_SURRENDER: {
        return action_surrender(game);
    }
    case KEY_ACTION: {
        const bool error = action_attack(game) && action_select(game) &&
                           action_highlight(game) && action_move(game);
        if (!error)
            game_deselect(game);
        return false;
    }
    case '1': {
        return parse_1(game);
    }
    case '2': {
        return game_load(game, "state2.txt");
    }
    case '3': {
        return game_load(game, "state3.txt");
    }
    case '4': {
        return game_load(game, "state4.txt");
    }
    case '5': {
        return game_load(game, "state5.txt");
    }
    case '6': {
        return game_save(game, "state1.txt");
    }
    case '7': {
        return game_save(game, "state2.txt");
    }
    case '8': {
        return game_save(game, "state3.txt");
    }
    case '9': {
        return game_save(game, "state4.txt");
    }
    case '0': {
        return game_save(game, "state5.txt");
    }
    }

    return false;
}