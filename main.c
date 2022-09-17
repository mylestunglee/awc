#include "console.h"
#include "controller.h"
#include "game.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: awc GAME_STATE\n");
        return 1;
    }

    struct game* const game = (struct game*)malloc(sizeof(struct game));

    if (game == NULL) {
        fprintf(stderr, "Insufficent memory\n");
        return 1;
    }

    const bool error = game_load(game, argv[1]);

    if (error) {
        fprintf(stderr, "Failed to load GAME_STATE at \"%s\"\n", argv[1]);
        free(game);
        return 1;
    }

    controller_run(game, getch);
    free(game);
    return 0;
}
