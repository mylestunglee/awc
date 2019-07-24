#include <stdlib.h>
#include "game.h"
#include "graphics.h"
#include "bitarray.h"

int main() {
	struct game* const game = malloc(sizeof(struct game));

	game_load(game, "state1.txt");
	game_loop(game);
	reset_style();

	free(game);
	return 0;
}
