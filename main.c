#include <stdlib.h>
#include <stdio.h>
#include "game.h"
#include "optimise.h"

int main(int argc, char* argv[]) {
	struct game* const game = malloc(sizeof(struct game));

	health_wide_t friendly_distribution[model_capacity] = {0};
	health_wide_t enemy_distribution[model_capacity] = {0};
	tile_wide_t buildable_allocations[model_capacity] = {0};
	gold_t budget = 10;
	tile_wide_t build_allocations[model_capacity] = {0};
	optimise_build_allocations(
		friendly_distribution,
		enemy_distribution,
		buildable_allocations,
		budget,
		build_allocations,
		&game->list.nodes);

	return 0;

	if (argc != 2) {
		fprintf(stderr, "Usage: awc GAME_STATE\n");
		return 1;
	}


	if (game == NULL) {
		fprintf(stderr, "Insufficent memory\n");
		return 1;
	}

	const bool error = game_load(game, argv[1]);

	if (error) {
		fprintf(stderr, "Failed to load GAME_STATE\n");
		return 1;
	}

	game_loop(game);

	free(game);

	return 0;
}
