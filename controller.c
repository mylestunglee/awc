#include "controller.h"
#include "console.h"
#include "graphics.h"
#include "parse.h"
#include <stdio.h>

void controller_run(struct game* const game) {
    graphics_init();

    do {
        graphics_render(game);

        const char input = getch();

        if (input == KEY_QUIT)
            break;

        if (parse_command(game, input))
            printf("Command failed");

    } while (true);
}