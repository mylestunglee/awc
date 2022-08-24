#include "parse.h"
#include "action.h"
#include "constants.h"
#include "turn.h"
#include "unit_constants.h"
#include <ctype.h>

bool parse_error(struct game* const game) {
    (void)game;
    return true;
}

bool parse_nothing(struct game* const game) {
    (void)game;
    return false;
}

bool parse_build(struct game* const game, const model_t offset) {
    if (game->map[game->y][game->x] < TERRIAN_CAPACITY)
        return false;

    const tile_t capturable = game->map[game->y][game->x] - TERRIAN_CAPACITY;
    const model_t model = offset + buildable_models[capturable];

    return action_build(game, model);
}

bool parse_build_1(struct game* const game) { return parse_build(game, 0); }

bool parse_build_2(struct game* const game) { return parse_build(game, 1); }

bool parse_build_3(struct game* const game) { return parse_build(game, 2); }

bool parse_build_4(struct game* const game) { return parse_build(game, 3); }

bool parse_build_5(struct game* const game) { return parse_build(game, 4); }

bool parse_build_6(struct game* const game) { return parse_build(game, 5); }

bool parse_build_7(struct game* const game) { return parse_build(game, 6); }

bool parse_build_8(struct game* const game) { return parse_build(game, 7); }

bool parse_build_9(struct game* const game) { return parse_build(game, 8); }

bool parse_build_10(struct game* const game) { return parse_build(game, 9); }

bool parse_load_1(struct game* const game) {
    return game_load(game, "state1.txt");
}

bool parse_load_2(struct game* const game) {
    return game_load(game, "state2.txt");
}

bool parse_load_3(struct game* const game) {
    return game_load(game, "state3.txt");
}

bool parse_load_4(struct game* const game) {
    return game_load(game, "state4.txt");
}

bool parse_load_5(struct game* const game) {
    return game_load(game, "state5.txt");
}

bool parse_save_1(struct game* const game) {
    return game_save(game, "state1.txt");
}

bool parse_save_2(struct game* const game) {
    return game_save(game, "state2.txt");
}

bool parse_save_3(struct game* const game) {
    return game_save(game, "state3.txt");
}

bool parse_save_4(struct game* const game) {
    return game_save(game, "state4.txt");
}

bool parse_save_5(struct game* const game) {
    return game_save(game, "state5.txt");
}

bool parse_next_turn(struct game* const game) {
    turn_next(game);
    action_hover_next_unit(game);
    return false;
}

bool parse_pan_up(struct game* const game) {
    game->prev_x = game->x;
    game->prev_y = game->y;
    --game->y;
    return false;
}

bool parse_pan_left(struct game* const game) {
    game->prev_x = game->x;
    game->prev_y = game->y;
    --game->x;
    return false;
}

bool parse_pan_down(struct game* const game) {
    game->prev_x = game->x;
    game->prev_y = game->y;
    ++game->y;
    return false;
}

bool parse_pan_right(struct game* const game) {
    game->prev_x = game->x;
    game->prev_y = game->y;
    ++game->x;
    return false;
}

bool parse_action(struct game* const game) {
    if (action_attack(game) && action_select(game) && action_highlight(game) &&
        action_move(game)) {
        game_deselect(game);
        return false;
    }

    return false;
}

bool (*parsers[256])(struct game* const);

void parse_initialise(void) {
    uint8_t input = 0;
    do
    {
        parsers[input] = isprint(input) ? &parse_nothing : &parse_error;
    }
    while (++input);

    parsers[KEY_BULID_1] = &parse_build_1;
    parsers[KEY_BULID_2] = &parse_build_2;
    parsers[KEY_BULID_3] = &parse_build_3;
    parsers[KEY_BULID_4] = &parse_build_4;
    parsers[KEY_BULID_5] = &parse_build_5;
    parsers[KEY_BULID_6] = &parse_build_6;
    parsers[KEY_BULID_7] = &parse_build_7;
    parsers[KEY_BULID_8] = &parse_build_8;
    parsers[KEY_BULID_9] = &parse_build_9;
    parsers[KEY_BULID_10] = &parse_build_10;
    parsers[KEY_LOAD_1] = &parse_load_1;
    parsers[KEY_LOAD_2] = &parse_load_2;
    parsers[KEY_LOAD_3] = &parse_load_3;
    parsers[KEY_LOAD_4] = &parse_load_4;
    parsers[KEY_LOAD_5] = &parse_load_5;
    parsers[KEY_SAVE_1] = &parse_save_1;
    parsers[KEY_SAVE_2] = &parse_save_2;
    parsers[KEY_SAVE_3] = &parse_save_3;
    parsers[KEY_SAVE_4] = &parse_save_4;
    parsers[KEY_SAVE_5] = &parse_save_5;
    parsers[KEY_NEXT_TURN] = &parse_next_turn;
    parsers[KEY_PAN_UP] = &parse_pan_up;
    parsers[KEY_PAN_LFFT] = &parse_pan_left;
    parsers[KEY_PAN_DOWN] = &parse_pan_down;
    parsers[KEY_PAN_RIGHT] = &parse_pan_right;
    parsers[KEY_HOVER_NEXT_UNIT] = &action_hover_next_unit;
    parsers[KEY_SELF_DESTRUCT] = &action_self_destruct;
    parsers[KEY_SURRENDER] = &action_surrender;
    parsers[KEY_ACTION] = &parse_action;
}

bool parse_command(struct game* const game, const uint8_t input) {
    return (*parsers[input])(game);
}
