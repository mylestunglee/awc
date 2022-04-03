#define EXPOSE_BUILD_UNITS_INTERNAL
#include "../build_units.h"
#include "../constants.h"
#include "../unit_constants.h"
#include "game_fixture.hpp"
#include "test_constants.hpp"

TEST_F(game_fixture, accumulate_distribution) {
    health_wide_t distribution[MODEL_CAPACITY] = {0};
    insert_unit({.health = 2, .model = MODEL_INFANTRY});

    accumulate_distribution(game, game->turn, distribution);

    ASSERT_EQ(distribution[MODEL_INFANTRY], 2);
}

TEST_F(game_fixture, inputs_initialise_distributions) {
    struct bap_inputs inputs = {0};
    insert_unit({.health = 2, .model = MODEL_INFANTRY, .player = 0, .x = 1});
    insert_unit({.health = 3, .model = MODEL_INFANTRY, .player = 1, .x = 2});

    inputs_initialise_distributions(game, &inputs);

    ASSERT_EQ(inputs.friendly_distribution[MODEL_INFANTRY], 2);
    ASSERT_EQ(inputs.enemy_distribution[MODEL_INFANTRY], 3);
}

TEST_F(game_fixture, inputs_initialise_capturables) {
    struct bap_inputs inputs = {0};
    game->territory[3][2] = game->turn;
    game->map[3][2] = TILE_FACTORY;

    inputs_initialise_capturables(game, &inputs);

    ASSERT_EQ(inputs.capturables[CAPTURABLE_FACTORY], 1);
}

TEST_F(game_fixture, inputs_initialise) {
    struct bap_inputs inputs = {0};
    insert_unit({.health = 2, .model = MODEL_INFANTRY});
    game->territory[3][2] = game->turn;
    game->map[3][2] = TILE_FACTORY;
    game->golds[game->turn] = 5 * GOLD_SCALE;

    inputs_initialise(game, &inputs);

    ASSERT_EQ(inputs.friendly_distribution[MODEL_INFANTRY], 2);
    ASSERT_EQ(inputs.capturables[CAPTURABLE_FACTORY], 1);
    ASSERT_EQ(inputs.budget, 5 * GOLD_SCALE);
}

TEST_F(game_fixture, realise_allocations) {
    grid_wide_t allocations[MODEL_CAPACITY] = {0};
    allocations[MODEL_INFANTRY] = 1;
    game->territory[3][2] = game->turn;
    game->map[3][2] = TILE_FACTORY;
    game->golds[game->turn] = models_cost[MODEL_INFANTRY];

    realise_allocations(game, allocations);

    ASSERT_EQ(game->golds[game->turn], 0);
}

TEST_F(game_fixture, build_units) {
    game->territory[3][2] = game->turn;
    game->map[3][2] = TILE_FACTORY;
    game->golds[game->turn] = models_cost[MODEL_INFANTRY];

    build_units(game);

    ASSERT_EQ(game->golds[game->turn], 0);
}