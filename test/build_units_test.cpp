#include "../build_units.h"
#include "game_fixture.hpp"
#include "test_constants.hpp"

TEST_F(game_fixture, accumulate_distribution) {
    health_wide_t distribution[MODEL_CAPACITY] = {0};
    insert_unit({.health = 2, .model = MODEL_INFANTRY, .x = 1});
    insert_unit({.health = 3, .model = MODEL_INFANTRY, .x = 2});

    accumulate_distribution(game, game->turn, distribution);

    ASSERT_EQ(distribution[MODEL_INFANTRY], 5);
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
    game->map[3][2] = TERRIAN_CAPACITY + CAPTURABLE_FACTORY;

    inputs_initialise_capturables(game, &inputs);

    ASSERT_EQ(inputs.capturables[CAPTURABLE_FACTORY], 1);
}