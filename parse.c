#include "parse.h"
#include "file.h"
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

    return game_surrender(game);
}

bool parse_select_next_unit(struct game* const game, const char input) {
    return input == 'm' && game_select_next_unit(game);
}