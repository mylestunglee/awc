#include "parse.h"
#include "action.h"
#include "file.h"
#include "turn.h"
#include <stdio.h>

bool parse_file(struct game* const game, const char input) {
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

bool parse_panning(struct game* const game, const char input) {
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

bool parse_surrender(struct game* const game, const char input) {
    if (input != 'K')
        return false;

    return action_surrender(game);
}

bool parse_select_next_unit(struct game* const game, const char input) {
    return input == 'm' && game_hover_next_unit(game);
}

bool parse_build(struct game* const game, const char input) {
    const tile_t capturable = game->map[game->y][game->x] - terrian_capacity;
    const model_t value = input - '1';
    const model_t model = value + buildable_models[capturable];

    return !action_build(game, model);
}

bool parse_self_destruct_unit(struct game* const game, const char input) {
    if (input != 'k')
        return false;

    return action_self_destruct_selection(game);
}

bool parse_space(struct game* const game, const char input,
                 const bool attackable) {
    if (input != ' ')
        return false;

    if (attackable) {
        action_attack(game);
        return true;
    }

    return false;
}

bool parse_command(struct game* const game, const char input, bool attackable,
                   bool build_enabled) {

    if (input == 'q')
        return true;

    if (input == 'n') {
        turn_next(game);
        return false;
    }

    if (parse_panning(game, input))
        return false;

    if (parse_select_next_unit(game, input))
        return false;

    if (parse_self_destruct_unit(game, input))
        return false;

    if (parse_surrender(game, input))
        return false;

    // TOOD: inline build_enabled into parse_build
    if (build_enabled && parse_build(game, input))
        return false;

    if (parse_file(game, input))
        return false;

    // TODO: rename to parse_attack
    if (parse_space(game, input, attackable))
        return false;

    if (input == ' ' && action_select(game))
        return false;

    if (input == ' ' && action_highlight(game))
        return false;

    if (input == ' ' && action_move(game))
        return false;

    if (input == ' ') {
        game_deselect(game);
        return false;
    }

    return false;
}