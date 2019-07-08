#include <stdio.h>
#include "game.h"
#include "graphics.h"
#include "queue.h"

int main() {
	struct game game;
	game_initialise(&game);

	for (int i = 0; i < 15; i++) {
		units_insert(&game.units, (struct unit){.model = i, .x = i, .y = 3, .health = i * 18 + 3, .player = i % 2, .enabled = true});
	}

	game_loop(&game);

	reset_style();

	return 0;
}
