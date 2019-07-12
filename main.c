#include <stdio.h>
#include "game.h"
#include "graphics.h"
#include "queue.h"
#include "file.h"

int main() {
	struct game game;

	game_load(&game, "state1.txt");
	game_loop(&game);
	reset_style();
	return 0;
}
