#include "controller.h"
#include "console.h"
#include "graphics.h"
#include "parse.h"

void controller_run(struct game* const game) {
    do {
        const bool attack_enabled = game_attack_enabled(game);
        const bool build_enabled = game_build_enabled(game);
        assert(!(attack_enabled && build_enabled));

        render(game, attack_enabled, build_enabled);
        game_print_text(game, attack_enabled, build_enabled);

        const char input = getch();

        if (parse_command(game, input, attack_enabled, build_enabled))
            break;

    } while (true);
}