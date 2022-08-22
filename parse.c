#include "parse.h"
#include "action.h"
#include "constants.h"
#include "file.h"
#include "turn.h"
#include "unit_constants.h"
#include <stdio.h>

bool parse_command(struct game* const game, const char input) {
    // TODO: add error handling
    switch (input) {
    case KEY_QUIT: {
        return true;
    }
    case KEY_NEXT_TURN: {
        turn_next(game);
        action_hover_next_unit(game);
        break;
    }
    case KEY_PAN_UP: {
        game->prev_x = game->x;
        game->prev_y = game->y;
        --game->y;
        break;
    }
    case KEY_PAN_LFFT: {
        game->prev_x = game->x;
        game->prev_y = game->y;
        --game->x;
        break;
    }
    case KEY_PAN_DOWN: {
        game->prev_x = game->x;
        game->prev_y = game->y;
        ++game->y;
        break;
    }
    case KEY_PAN_RIGHT: {
        game->prev_x = game->x;
        game->prev_y = game->y;
        ++game->x;
        break;
    }
    case KEY_HOVER_NEXT_UNIT: {
        action_hover_next_unit(game);
        break;
    }
    case KEY_SELF_DESTRUCT: {
        action_self_destruct(game);
        break;
    }
    case KEY_SURRENDER: {
        action_surrender(game);
        break;
    }
    case KEY_ACTION: {
        const bool error = action_attack(game) && action_select(game) &&
                           action_highlight(game) && action_move(game);
        if (!error)
            game_deselect(game);
        break;
    }
    case '1': {
        return game_load(game, "state1.txt");
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
        return file_save(game, "state1.txt");
    }
    case '7': {
        return file_save(game, "state2.txt");
    }
    case '8': {
        return file_save(game, "state3.txt");
    }
    case '9': {
        return file_save(game, "state4.txt");
    }
    case '0': {
        return file_save(game, "state5.txt");
    }
    }

    return false;
}