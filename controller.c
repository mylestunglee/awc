#include "controller.h"
#include "console.h"
#include "graphics.h"
#include "parse.h"

void controller_run(struct game* const game) {
    graphics_init();

    do {
        const bool attackable = game_is_attackable(game);
        const bool buildable = game_is_buildable(game);
        assert(!(attackable && buildable));

        graphics_render(game, attackable, buildable);

        const char input = getch();

        if (parse_command(game, input, attackable, buildable))
            break;

    } while (true);
}