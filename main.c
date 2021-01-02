#include <stdlib.h>
#include <stdio.h>
#include "game.h"
#include "optimise.h"

int main(int argc, char* argv[]) {
	struct game* const game = malloc(sizeof(struct game));

	health_wide_t friendly_distribution[model_capacity] = {0};
	health_wide_t enemy_distribution[model_capacity] = {0};
	tile_wide_t capturables[capturable_capacity] = {0};
	gold_t budget = 0;
	double build_allocations[model_capacity] = {0};

	budget = 16;
	enemy_distribution[4] = 1;
	enemy_distribution[10] = 1;
	friendly_distribution[3] = 40;
	capturables[3] = 2;

	optimise_build_allocations(
		friendly_distribution,
		enemy_distribution,
		capturables,
		budget,
		build_allocations,
		&game->list.nodes);

	printf("result=");
	for (model_t m = 0; m < model_capacity; ++m)
		printf("(%d) %s %f\n", m, model_names[m], build_allocations[m]);
	printf("\n");

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
