#define EXPOSE_FILE_INTERNALS
#include "../constants.h"
#include "../file.h"
#include "game_fixture.hpp"
#include "test_constants.hpp"
#include "units_fixture.hpp"
#include <string>

using namespace std::literals::string_literals;

TEST(file_test, load_turn_when_invalid_command) {
    ASSERT_FALSE(load_turn("", "", nullptr));
}

TEST(file_test, load_turn_when_valid_sets_turn) {
    player_t turn;

    ASSERT_TRUE(load_turn("turn", "2", &turn));

    ASSERT_EQ(turn, 2);
}

TEST(file_test, load_map_when_invalid_command) {
    ASSERT_FALSE(load_map("", "", 0, nullptr));
}

TEST(file_test, load_map_when_valid_sets_map) {
    tile_t map[GRID_SIZE][GRID_SIZE];
    grid_t y = 2;

    ASSERT_TRUE(load_map("map", "\"", &y, map));

    ASSERT_EQ(map[2][0], TILE_PLAINS);
    ASSERT_EQ(y, 3);
}

TEST(file_test, load_territory_when_invalid_command) {
    ASSERT_FALSE(load_territory("", "", nullptr));
}

TEST(file_test, load_territory_when_invalid_params) {
    ASSERT_FALSE(load_territory("territory", "", nullptr));
}

TEST(file_test, load_territory_when_invalid_player) {
    const auto params = std::to_string(NULL_PLAYER) + " 0 0"s;

    ASSERT_FALSE(load_territory("territory", params.c_str(), nullptr));
}

TEST(file_test, load_territory_when_valid_sets_territory) {
    player_t territory[GRID_SIZE][GRID_SIZE];

    ASSERT_TRUE(load_territory("territory", "2 3 5", territory));

    ASSERT_EQ(territory[5][3], 2);
}

TEST(file_test, load_bot_when_invalid_command) {
    ASSERT_FALSE(load_bot("", "", nullptr));
}

TEST(file_test, load_bot_when_invalid_params) {
    ASSERT_FALSE(load_bot("bot", "", nullptr));
}

TEST(file_test, load_bot_when_invalid_player) {
    const auto params = std::to_string(NULL_PLAYER);

    ASSERT_FALSE(load_bot("bot", params.c_str(), nullptr));
}

TEST(file_test, load_bot_when_valid_sets_bots) {
    uint8_t bots = '\x00';

    ASSERT_TRUE(load_bot("bot", "2", &bots));

    ASSERT_EQ(bots, '\x01' << 2);
}

TEST(file_test, load_gold_when_invalid_command) {
    ASSERT_FALSE(load_gold("", "", nullptr));
}

TEST(file_test, load_gold_when_invalid_params) {
    ASSERT_FALSE(load_gold("gold", "", nullptr));
}

TEST(file_test, load_gold_when_invalid_player) {
    const auto params = std::to_string(NULL_PLAYER) + " 0"s;

    ASSERT_FALSE(load_gold("gold", params.c_str(), nullptr));
}

TEST(file_test, load_gold_when_valid_sets_bots) {
    gold_t golds[PLAYERS_CAPACITY];

    ASSERT_TRUE(load_gold("gold", "2 3", golds));

    ASSERT_EQ(golds[2], 3);
}

TEST(file_test, load_team_when_invalid_command) {
    auto params = ""s;

    ASSERT_FALSE(load_team("", params.data(), nullptr));
}

TEST(file_test, load_team_when_invalid_params) {
    auto params = "player"s;

    ASSERT_FALSE(load_team("team", params.data(), nullptr));
}

TEST(file_test, load_team_when_invalid_player) {
    auto params = std::to_string(NULL_PLAYER);

    ASSERT_FALSE(load_team("team", params.data(), nullptr));
}

TEST(file_test, load_team_when_no_params) {
    auto params = ""s;

    ASSERT_FALSE(load_team("team", params.data(), nullptr));
}

TEST(file_test, load_team_when_valid_sets_alliances) {
    auto params = "0 1"s;
    uint8_t alliances = '\x00';

    ASSERT_TRUE(load_team("team", params.data(), &alliances));

    ASSERT_EQ(alliances, '\x01');
}

TEST(file_test, load_unit_when_invalid_command) {
    ASSERT_FALSE(load_unit("", "", 0, nullptr));
}

TEST(file_test, load_unit_when_invalid_params) {
    ASSERT_FALSE(load_unit("infantry", "", MODEL_INFANTRY, nullptr));
}

TEST(file_test, load_unit_when_invalid_player) {
    const auto params = std::to_string(NULL_PLAYER) + " 0 0 0 enabled"s;

    ASSERT_FALSE(
        load_unit("infantry", params.c_str(), MODEL_INFANTRY, nullptr));
}

TEST(file_test, load_unit_when_invalid_enabled) {
    const auto params = "2 3 5 7 abled"s;

    ASSERT_FALSE(
        load_unit("infantry", params.c_str(), MODEL_INFANTRY, nullptr));
}

TEST_F(units_fixture, load_unit_when_valid_inserts_enabled_unit) {
    const auto params = "2 3 5 7 enabled"s;

    ASSERT_TRUE(load_unit("infantry", params.c_str(), MODEL_INFANTRY, units));

    const auto* const unit = units_const_get_at(units, 3, 5);
    ASSERT_TRUE(unit);
    ASSERT_EQ(unit->player, 2);
    ASSERT_EQ(unit->health, 7);
    ASSERT_TRUE(unit->enabled);
}

TEST_F(units_fixture, load_unit_when_valid_inserts_disabled_unit) {
    const auto params = "2 3 5 7 disabled"s;

    ASSERT_TRUE(load_unit("infantry", params.c_str(), MODEL_INFANTRY, units));

    const auto* const unit = units_const_get_at(units, 3, 5);
    ASSERT_TRUE(unit);
    ASSERT_FALSE(unit->enabled);
}
