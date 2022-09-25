#define EXPOSE_FILE_INTERNALS
#include "../constants.h"
#include "../file.h"
#include "game_fixture.hpp"
#include "test_constants.hpp"
#include "units_fixture.hpp"
#include <filesystem>
#include <fstream>
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

TEST_F(units_fixture, load_unit_when_unspecified_health_returns_true) {
    const auto params = "2 3 5"s;

    ASSERT_TRUE(load_unit("infantry", params.c_str(), MODEL_INFANTRY, units));

    const auto* const unit = units_const_get_at(units, 3, 5);
    ASSERT_EQ(unit->health, HEALTH_MAX);
}

TEST_F(units_fixture, load_unit_when_invalid_health_returns_false) {
    const auto params = "2 3 5 a"s;

    ASSERT_FALSE(load_unit("infantry", params.c_str(), MODEL_INFANTRY, units));
}

TEST_F(units_fixture, load_unit_when_valid_inserts_default_enabled_unit) {
    const auto params = "0 3 5 7"s;

    ASSERT_TRUE(load_unit("infantry", params.c_str(), MODEL_INFANTRY, units));

    const auto* const unit = units_const_get_at(units, 3, 5);
    ASSERT_TRUE(unit);
    ASSERT_EQ(unit->player, 0);
    ASSERT_EQ(unit->health, 7);
    ASSERT_TRUE(unit->enabled);
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

TEST_F(units_fixture, load_units_when_valid_unit) {
    const auto params = "2 3 5 7 enabled"s;

    ASSERT_TRUE(load_units("infantry", params.c_str(), units));

    ASSERT_TRUE(units_const_get_at(units, 3, 5));
}

TEST(file_test, load_units_when_invalid_unit) {
    ASSERT_FALSE(load_units("", nullptr, nullptr));
}

TEST_F(game_fixture, load_command_returns_false_when_invalid_command) {
    grid_t y;

    ASSERT_FALSE(load_command(game, "", nullptr, &y));
}

TEST_F(game_fixture, file_load_returns_false_when_valid_contents) {
    using namespace std;
    const auto filename = "file1.txt";
    {
        ofstream file(filename);
        file << "map \"";
    }

    ASSERT_FALSE(file_load(game, filename));

    ASSERT_EQ(game->map[0][0], TILE_PLAINS);
    remove(filename);
}

TEST_F(game_fixture, file_load_returns_true_when_invalid_contents) {
    using namespace std;
    const auto filename = "file2.txt";
    {
        ofstream file(filename);
        file << "command \"";
    }

    ASSERT_TRUE(file_load(game, filename));

    remove(filename);
}

TEST_F(game_fixture, file_load_returns_true_when_file_does_not_exist) {
    ASSERT_TRUE(file_load(game, ""));
}

TEST(file_test, calc_row_length_when_empty_row) {
    const tile_t row[GRID_SIZE] = {0};

    ASSERT_EQ(calc_row_length(row), 0);
}

TEST(file_test, calc_row_length_when_leftmost_tile_is_not_void) {
    tile_t row[GRID_SIZE] = {0};
    row[0] = TILE_PLAINS;

    ASSERT_EQ(calc_row_length(row), 1);
}

TEST(file_test, calc_row_length_when_rightmost_is_not_void) {
    tile_t row[GRID_SIZE] = {0};
    row[GRID_SIZE - 1] = TILE_PLAINS;

    ASSERT_EQ(calc_row_length(row), GRID_SIZE);
}

TEST(file_test, calc_row_count_when_empty_map) {
    const tile_t map[GRID_SIZE][GRID_SIZE] = {0};

    ASSERT_EQ(calc_row_count(map), 0);
}

TEST(file_test, calc_row_length_when_upmost_row_is_not_void) {
    tile_t map[GRID_SIZE][GRID_SIZE] = {0};
    map[0][0] = TILE_PLAINS;

    ASSERT_EQ(calc_row_count(map), 1);
}

TEST(file_test, calc_row_length_when_downmost_row_is_not_void) {
    tile_t map[GRID_SIZE][GRID_SIZE] = {0};
    map[GRID_SIZE - 1][0] = TILE_PLAINS;

    ASSERT_EQ(calc_row_count(map), GRID_SIZE);
}

class file_fixture {
public:
    file_fixture() { file = open_memstream(&buffer, &size); }

    ~file_fixture() {
        fclose(file);
        free(buffer);
    }

    FILE* ref() { return file; }

    std::string data() {
        fflush(file);
        return std::string(buffer);
    }

private:
    FILE* file = nullptr;
    char* buffer = nullptr;
    size_t size = 0;
};

TEST(file_test, save_turn) {
    file_fixture file;

    save_turn(2, file.ref());

    ASSERT_EQ(file.data(), "turn 2\n");
}

TEST(file_test, save_map_when_no_empty_rows) {
    file_fixture file;
    tile_t map[GRID_SIZE][GRID_SIZE] = {0};
    map[0][0] = TILE_PLAINS;

    save_map(map, file.ref());

    ASSERT_EQ(file.data(), "map \"\n");
}

TEST(file_test, save_map_when_empty_row) {
    file_fixture file;
    tile_t map[GRID_SIZE][GRID_SIZE] = {0};
    map[1][0] = TILE_PLAINS;

    save_map(map, file.ref());

    ASSERT_EQ(file.data(), "map\nmap \"\n");
}

TEST(file_test, save_unit_no_capture_progress_and_no_health) {
    file_fixture file;
    const struct unit unit {
        .x = 5, .y = 7, .health = HEALTH_MAX, .enabled = true
    };

    save_unit(&unit, file.ref());

    ASSERT_EQ(file.data(), "infantry     0   5   7\n");
}

TEST(file_test, save_unit_no_capture_progress_and_no_enabled) {
    file_fixture file;
    const struct unit unit {
        .x = 5, .y = 7, .health = 2,
        .enabled = true
    };

    save_unit(&unit, file.ref());

    ASSERT_EQ(file.data(), "infantry     0   5   7   2\n");
}

TEST(file_test, save_unit_no_capture_progress) {
    file_fixture file;
    const struct unit unit {
        .x = 5, .y = 7, .player = 3, .health = 2, 
        .enabled = true
    };

    save_unit(&unit, file.ref());

    ASSERT_EQ(file.data(), "infantry     3   5   7   2  enabled\n");
}

TEST(file_test, save_unit_all_fields) {
    file_fixture file;
    const struct unit unit {
        .x = 5, .y = 7, .player = 3, .health = 2, .capture_progress = 11, .enabled = true
    };

    save_unit(&unit, file.ref());

    ASSERT_EQ(file.data(), "infantry     3   5   7   2  enabled 11\n");
}

TEST_F(units_fixture, save_units) {
    file_fixture file;
    insert({.x = 2, .y = 3});
    insert({.x = 5, .y = 7});

    save_units(units, file.ref());

    auto new_lines = 0;
    for (const auto c : file.data())
        if (c == '\n')
            ++new_lines;

    ASSERT_EQ(new_lines, 2);
}

TEST(file_test, save_territory) {
    file_fixture file;
    player_t territory[GRID_SIZE][GRID_SIZE];
    memset(territory, NULL_PLAYER, sizeof territory);
    territory[3][2] = 0;

    save_territory(territory, file.ref());

    ASSERT_EQ(file.data(), "territory 0   2   3\n");
}

TEST(file_test, save_golds) {
    file_fixture file;
    gold_t golds[PLAYERS_CAPACITY] = {0};
    golds[2] = 3;

    save_golds(golds, file.ref());

    ASSERT_EQ(file.data(), "gold 2 3\n");
}

TEST(file_test, save_bots) {
    file_fixture file;
    uint8_t bots = '\x02';

    save_bots(&bots, file.ref());

    ASSERT_EQ(file.data(), "bot 1\n");
}

TEST(file_test, save_teams) {
    file_fixture file;
    uint8_t alliances[BITMATRIX_SIZE(PLAYERS_CAPACITY)];

    alliances[0] = (1 << 1) + (1 << 2) + (1 << 7);

    save_teams(alliances, file.ref());

    ASSERT_EQ(file.data(), "team 0 2 3\n");
}

TEST_F(game_fixture, save_game) {
    file_fixture file;
    game->map[0][0] = TILE_PLAINS;
    insert_unit();
    game->territory[0][0] = 0;
    game->golds[0] = 1;
    bitarray_set(game->bots, 0);
    bitmatrix_set(game->alliances, 0, 1);

    save_game(game, file.ref());

    auto new_lines = 0;
    for (const auto c : file.data())
        if (c == '\n')
            ++new_lines;

    ASSERT_EQ(new_lines, 7);
}

TEST_F(game_fixture, file_save_returns_true_when_invalid_filename) {
    ASSERT_TRUE(file_save(nullptr, ""));
}

TEST_F(game_fixture, file_save_returns_false_when_successful) {
    ASSERT_FALSE(file_save(game, "savegame"));

    ASSERT_EQ(std::filesystem::file_size("savegame"), 7);

    remove("savegame");
}