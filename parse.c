#include "parse.h"
#include "action.h"
#include "constants.h"
#include "file.h"
#include "turn.h"
#include "unit_constants.h"
#include <stdio.h>

#define KEY_QUIT 'q'
#define KEY_NEXT_TURN 'n'
#define KEY_PAN_UP 'w'
#define KEY_PAN_LFFT 'a'
#define KEY_PAN_DOWN 's'
#define KEY_PAN_RIGHT 'd'
#define KEY_SELECT_NEXT_UNIT 'm'
#define KEY_SELF_DESTRUCT 'k'
#define KEY_SURRENDER 'K'
#define KEY_ACTION ' '

bool parse_quit(const char input) { return input == KEY_QUIT; }

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

bool parse_next_turn(struct game* const game, const char input) {
    if (input == KEY_NEXT_TURN) {
        turn_next(game);
        return true;
    }

    return false;
}

bool parse_panning(struct game* const game, const char input) {
    switch (input) {
    case KEY_PAN_UP: {
        game->prev_x = game->x;
        game->prev_y = game->y;
        --game->y;
        return true;
    }
    case KEY_PAN_LFFT: {
        game->prev_x = game->x;
        game->prev_y = game->y;
        --game->x;
        return true;
    }
    case KEY_PAN_DOWN: {
        game->prev_x = game->x;
        game->prev_y = game->y;
        ++game->y;
        return true;
    }
    case KEY_PAN_RIGHT: {
        game->prev_x = game->x;
        game->prev_y = game->y;
        ++game->x;
        return true;
    }
    }
    return false;
}

bool parse_select_next_unit(struct game* const game, const char input) {
    return input == KEY_SELECT_NEXT_UNIT &&
           (game_hover_next_unit(game) || true);
}

bool parse_self_destruct(struct game* const game, const char input) {
    return input == KEY_SELF_DESTRUCT && (action_self_destruct(game) || true);
}

bool parse_surrender(struct game* const game, const char input) {
    return input == KEY_SURRENDER && (action_surrender(game) || true);
}

bool parse_build(struct game* const game, const char input,
                 const bool buildable) {
    if (!buildable)
        return false;

    const tile_t capturable = game->map[game->y][game->x] - TERRIAN_CAPACITY;
    const model_t value = input - '1';
    const model_t model = value + buildable_models[capturable];

    return !action_build(game, model);
}

bool parse_attack(struct game* const game, const bool attackable) {
    if (attackable) {
        action_attack(game);
        return true;
    }

    return false;
}

bool parse_deselect(struct game* const game) {
    game_deselect(game);
    return true;
}

bool parse_space(struct game* const game, const char input,
                 const bool attackable) {
    return input == KEY_ACTION &&
           (parse_attack(game, attackable) || action_select(game) ||
            action_highlight(game) || action_move(game) ||
            parse_deselect(game));
}

// Returns true if needs to quit
bool parse_command(struct game* const game, const char input) {
    const bool attackable = game_is_attackable(game);
    const bool buildable = game_is_buildable(game);

    if (parse_quit(input) || parse_file(game, input))
        return true;

    (void)(parse_next_turn(game, input) || parse_panning(game, input) ||
           parse_select_next_unit(game, input) ||
           parse_self_destruct(game, input) || parse_surrender(game, input) ||
           parse_build(game, input, buildable) ||
           parse_space(game, input, attackable));

    return false;
}