#include "controller.h"
#include "console.h"
#include "graphics.h"
#include "parse.h"
#include <stdio.h>

void controller_run(struct game* const game) {
    parse_initialise();
    graphics_initialise();

    do {
        graphics_render(game);

        const char input = getch();

        if (input == 'q')
            break;

        if (parse_command(game, input)) {
            break;
            printf("Command failed");
        }

    } while (true);
}