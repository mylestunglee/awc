#include "game.h"
#include "asserts.h"
#include "graphics.h"

int main() {
	struct game game;

	game_load(&game, "state1.txt");
	game_loop(&game);
	reset_style();
	return 0;
}
