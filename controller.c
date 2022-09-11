#include "graphics.h"
#include "parse.h"
#include <stdio.h>

void controller_run(struct game* const game, char (*getch)(void)) {
    parse_initialise();
    graphics_initialise();

    do {
        graphics_render(game);

        const char input = getch();

        if (input == 'q' || parse_command(game, input))
            break;

    } while (true);
}