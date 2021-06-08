#include "parse.h"
#include "file.h"
#include <stdio.h>

#include "console.h"
#include "graphics.h"
#include "action.h"

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

bool parse_build(struct game* const game, const char input) {
    const tile_t capturable = game->map[game->y][game->x] - terrian_capacity;
    const model_t value = input - '1';
    const model_t model = value + buildable_models[capturable];

    return !action_build(game, model);
}

bool parse_self_destruct_unit(struct game* const game,
                                     const char input) {
    if (input != 'k')
        return false;

    return action_self_destruct_selection(game);
}

bool parse_space(struct game* const game, const char input, const bool attack_enabled) {
    if (input != ' ')
        return false;

    if (attack_enabled)
        game_attack(game);
    else
        game_handle_unit_selection(game);  
    return true;
}

void parse_main(struct game* const game) {
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

        if (parse_self_destruct_unit(game, input))
            continue;

        if (parse_surrender(game, input))
            continue;

        if (build_enabled && parse_build(game, input))
            continue;

        if (parse_file(game, input))
            continue;

        if (parse_space(game, input, attack_enabled))
            continue;
        
    } while (true);
}