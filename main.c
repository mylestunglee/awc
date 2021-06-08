#include "controller.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: awc GAME_STATE\n");
        return 1;
    }

    struct game* const game = malloc(sizeof(struct game));

    if (game == NULL) {
        fprintf(stderr, "Insufficent memory\n");
        return 1;
    }

    const bool error = game_load(game, argv[1]);

    if (error) {
        fprintf(stderr, "Failed to load GAME_STATE\n");
        return 1;
    }

    controller_run(game);

    free(game);

    return 0;
}
