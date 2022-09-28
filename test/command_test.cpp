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

    ASSERT_LT(attacker->health, HEALTH_MAX);
    ASSERT_LT(attackee->health, HEALTH_MAX);
    ASSERT_LT(attackee->health, attacker->health);
}