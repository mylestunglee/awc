#include "graphics.h"
#include "parse.h"
#include <stdio.h>

void controller_run(struct game* const game, char (*read_command)(void)) {
    parse_initialise();
    graphics_initialise();

    do {
        graphics_render(game);

        const char command = read_command();

        if (command == 'q' || parse_command(game, command))
            break;

    } while (true);
}
