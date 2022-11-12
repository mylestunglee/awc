#define EXPOSE_BUILD_UNITS_INTERNAL
#include "../build_units.h"
#include "../constants.h"
#include "../unit_constants.h"
#include "game_fixture.hpp"
#include "test_constants.hpp"

TEST_F(game_fixture, accumulate_distribution) {
    health_wide_t distribution[MODEL_CAPACITY] = {0};
    insert_unit({.health = 2});

    accumulate_distribution(game, game->turn, distribution);

    ASSERT_EQ(distribution[MODEL_INFANTRY], 2);
}

TEST_F(game_fixture, inputs_initialise_distributions) {
    struct bap_inputs inputs = {0};
    insert_unit({.x = 1, .health = 2});
    insert_unit({.x = 2, .player = 1, .health = 3});

    inputs_initialise_distributions(game, &inputs);

    ASSERT_EQ(inputs.friendly_distribution[MODEL_INFANTRY], 2);
    ASSERT_EQ(inputs.enemy_distribution[MODEL_INFANTRY], 3);
}

TEST_F(game_fixture, inputs_initialise_capturables) {
    struct bap_inputs inputs = {0};
    game->territory[3][2] = 0;
    game->map[3][2] = TILE_FACTORY;
    game->golds[0] = GOLD_SCALE;

    inputs_initialise_capturables(game, &inputs);

    ASSERT_EQ(inputs.capturables[CAPTURABLE_FACTORY], 1);
}

TEST_F(game_fixture, inputs_initialise) {
    struct bap_inputs inputs = {0};
    insert_unit({.health = 2});
    game->territory[3][2] = 0;
    game->map[3][2] = TILE_FACTORY;
    game->golds[0] = 5 * GOLD_SCALE;

    inputs_initialise(game, &inputs);

    ASSERT_EQ(inputs.friendly_distribution[MODEL_INFANTRY], 2);
    ASSERT_EQ(inputs.capturables[CAPTURABLE_FACTORY], 1);
    ASSERT_EQ(inputs.budget, 5 * GOLD_SCALE);
}

TEST_F(game_fixture, realise_allocations_builds_one_unit) {
    grid_wide_t allocations[MODEL_CAPACITY] = {0};
    allocations[MODEL_INFANTRY] = 1;
    game->territory[3][2] = 0;
    game->map[3][2] = TILE_FACTORY;
    game->golds[0] = model_cost[MODEL_INFANTRY];

    realise_allocations(game, allocations);

    ASSERT_EQ(allocations[MODEL_INFANTRY], 0);
    ASSERT_EQ(game->golds[0], 0);
}

TEST_F(game_fixture, realise_allocations_builds_no_units_when_no_allocations) {
    grid_wide_t allocations[MODEL_CAPACITY] = {0};
    game->territory[3][2] = 0;
    game->map[3][2] = TILE_FACTORY;

    realise_allocations(game, allocations);

    ASSERT_EQ(game->units.size, 0);
}

TEST_F(game_fixture, realise_allocations_builds_no_units_when_full) {
    game->units.size = UNITS_CAPACITY;
    grid_wide_t allocations[MODEL_CAPACITY] = {0};
    allocations[MODEL_INFANTRY] = 1;
    game->territory[3][2] = 0;
    game->map[3][2] = TILE_FACTORY;

    realise_allocations(game, allocations);

    ASSERT_FALSE(units_exists(&game->units, 2, 3));
}

TEST_F(game_fixture, build_units) {
    game->territory[3][2] = 0;
    game->map[3][2] = TILE_FACTORY;
    game->golds[0] = model_cost[MODEL_INFANTRY];

    build_units(game);

    ASSERT_EQ(game->golds[0], 0);
}
