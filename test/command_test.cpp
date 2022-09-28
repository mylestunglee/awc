#include "../constants.h"
#include "../parse.h"
#include "game_fixture.hpp"
#include "test_constants.hpp"

namespace {

void parse_commands(struct game* const game, const std::string& commands) {
    parse_initialise();
    for (const auto command : commands)
        parse_command(game, command);
}

} // namespace

TEST_F(game_fixture, capture_city) {
    game->map[0][0] = TILE_PLAINS;
    game->map[0][1] = TILE_CITY;
    insert_unit({.health = HEALTH_MAX, .enabled = true});

    parse_commands(game, " d n  ");

    ASSERT_EQ(game->territory[0][1], 0);
}

TEST_F(game_fixture, attack_enemy) {
    game->map[0][0] = TILE_PLAINS;
    game->map[0][1] = TILE_PLAINS;
    const auto* attacker = insert_unit({.health = HEALTH_MAX, .enabled = true});
    const auto* attackee =
        insert_unit({.x = 1, .player = 1, .health = HEALTH_MAX});

    parse_commands(game, " d ");

    ASSERT_EQ(attacker->health, 75);
    ASSERT_EQ(attackee->health, 51);
}

TEST_F(game_fixture, merge_both_attack_enemy) {
    game->map[0][0] = TILE_PLAINS;
    game->map[0][1] = TILE_PLAINS;
    game->map[0][2] = TILE_PLAINS;
    insert_unit({.health = HEALTH_MAX / 2, .enabled = true});
    insert_unit({.x = 1, .health = HEALTH_MAX / 2, .enabled = true});
    insert_unit({.x = 2, .player = 1, .health = HEALTH_MAX});

    parse_commands(game, " dd ");

    ASSERT_EQ(units_const_get_at(&game->units, 1, 0)->health, 75);
    ASSERT_EQ(units_const_get_at(&game->units, 2, 0)->health, 51);
}

TEST_F(game_fixture, merge_one_attack_enemy) {
    game->map[0][0] = TILE_PLAINS;
    game->map[0][1] = TILE_PLAINS;
    game->map[0][2] = TILE_PLAINS;
    insert_unit({.health = HEALTH_MAX / 2, .enabled = true});
    insert_unit({.x = 1, .health = HEALTH_MAX / 2});
    insert_unit({.x = 2, .player = 1, .health = HEALTH_MAX});

    parse_commands(game, " dd ");

    ASSERT_EQ(units_const_get_at(&game->units, 1, 0)->health, 63);
    ASSERT_EQ(units_const_get_at(&game->units, 2, 0)->health, 76);
}

TEST_F(game_fixture, bot_builds_infantry_after_continuing_turn) {
    game->map[0][0] = TILE_FACTORY;
    game->territory[0][0] = 0;
    game->golds[0] = 1000;
    bitarray_set(game->bots, 0);

    parse_commands(game, "n");

    auto* infantry = units_const_get_at(&game->units, 0, 0);
    ASSERT_TRUE(infantry);
    ASSERT_EQ(infantry->model, MODEL_INFANTRY);
}
