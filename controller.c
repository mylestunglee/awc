#include "controller.h"
#include "console.h"
#include "graphics.h"
#include "parse.h"

void controller_run(struct game* const game) {
    graphics_init();

    do {
        graphics_render(game);

        const char input = getch();

        if (parse_command(game, input))
            break;

    } while (true);
}