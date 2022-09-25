#define EXPOSE_UNITS_INTERNALS
#include "units_fixture.hpp"

TEST_F(units_fixture, units_initialise_sets_start) {
    units->start = 2;
    units_initialise(units);
    ASSERT_EQ(units->start, 0);
}

TEST_F(units_fixture, units_initialise_sets_size) {
    units->size = 2;
    units_initialise(units);
    ASSERT_EQ(units->size, 0);
}

TEST_F(units_fixture, units_initialise_sets_frees) {
    units->frees[2] = 5;
    units_initialise(units);
    ASSERT_EQ(units->frees[2], 3);
}

TEST_F(units_fixture, units_initialise_sets_firsts) {
    units->firsts[2] = 3;
    units_initialise(units);
    ASSERT_EQ(units->firsts[2], NULL_UNIT);
}

TEST_F(units_fixture, units_initialise_clears_grid) {
    units->grid[2][3] = 5;
    units_initialise(units);
    ASSERT_EQ(units->grid[2][3], NULL_UNIT);
}

TEST_F(units_fixture, insert_with_frees_sets_next_start) {
    units->start = 2;
    struct unit unit {};
    insert_with_frees(units, &unit);
    ASSERT_EQ(units->start, 3);
    ASSERT_EQ(units->frees[2], NULL_UNIT);
}

TEST_F(units_fixture, insert_with_frees_inserts_unit) {
    struct unit unit {
        .x = 2, .y = 3
    };
    auto index = insert_with_frees(units, &unit);
    ASSERT_NE(index, NULL_UNIT);
    ASSERT_EQ(units->data[index].x, 2);
    ASSERT_EQ(units->data[index].y, 3);
}

TEST_F(units_fixture, insert_with_frees_fails_when_full) {
    struct unit unit {};
    units->size = UNITS_CAPACITY;
    auto index = insert_with_frees(units, &unit);
    ASSERT_EQ(index, NULL_UNIT);
}

TEST_F(units_fixture, insert_with_players_sets_links_on_first_unit) {
    units->start = 2;
    struct unit unit {
        .player = 3
    };
    auto index = insert_with_players(units, &unit);
    ASSERT_EQ(index, 2);
    ASSERT_EQ(units->nexts[index], NULL_UNIT);
    ASSERT_EQ(units->prevs[index], NULL_UNIT);
    ASSERT_EQ(units->firsts[3], 2);
}

TEST_F(units_fixture, insert_with_players_set_links_second_unit) {
    units->start = 3;
    struct unit first_unit {
        .player = 2
    };
    struct unit second_unit {
        .player = 2
    };
    auto first_index = insert_with_players(units, &first_unit);
    auto second_index = insert_with_players(units, &second_unit);
    ASSERT_EQ(first_index, 3);
    ASSERT_EQ(second_index, 4);
    ASSERT_EQ(units->nexts[first_index], NULL_UNIT);
    ASSERT_EQ(units->nexts[second_index], 3);
    ASSERT_EQ(units->prevs[first_index], 4);
    ASSERT_EQ(units->prevs[second_index], NULL_UNIT);
    ASSERT_EQ(units->firsts[2], 4);
}

TEST_F(units_fixture, insert_with_players_propogates_failure) {
    struct unit unit;
    units->size = UNITS_CAPACITY;
    auto index = insert_with_players(units, &unit);
    ASSERT_EQ(index, NULL_UNIT);
}

TEST_F(units_fixture, units_insert_sets_grid) {
    struct unit unit {
        .x = 2, .y = 3
    };
    auto error = units_insert(units, &unit);
    ASSERT_FALSE(error);
    auto index = units->grid[3][2];
    ASSERT_NE(index, NULL_UNIT);
    auto& unit_ref = units->data[index];
    ASSERT_EQ(unit_ref.x, 2);
    ASSERT_EQ(unit_ref.y, 3);
}

TEST_F(units_fixture, units_insert_propogates_failure) {
    struct unit unit {};
    units->size = UNITS_CAPACITY;
    auto error = units_insert(units, &unit);
    ASSERT_TRUE(error);
}

TEST_F(units_fixture, delete_with_frees_links_free_index) {
    units->size = 2;
    units->start = 3;
    units->frees[5] = NULL_UNIT;
    delete_with_frees(units, 5);
    ASSERT_EQ(units->frees[5], 3);
    ASSERT_EQ(units->start, 5);
    ASSERT_EQ(units->size, 1);
}

TEST_F(units_fixture, delete_with_players_deletes_first_unit) {
    units->start = 2;
    struct unit unit {
        .player = 3
    };
    auto first_index = insert_with_players(units, &unit);
    auto second_index = insert_with_players(units, &unit);
    delete_with_players(units, first_index);
    ASSERT_EQ(units->firsts[3], second_index);
}

TEST_F(units_fixture, delete_with_players_deletes_middle_unit) {
    units->start = 2;
    struct unit unit {
        .player = 3
    };
    auto first_index = insert_with_players(units, &unit);
    auto second_index = insert_with_players(units, &unit);
    auto third_index = insert_with_players(units, &unit);
    delete_with_players(units, second_index);
    ASSERT_EQ(units->prevs[first_index], third_index);
    ASSERT_EQ(units->nexts[third_index], first_index);
}

TEST_F(units_fixture, units_delete_unsets_grid) {
    struct unit unit {
        .x = 2, .y = 3
    };
    units_insert(units, &unit);
    units_delete(units, units->grid[3][2]);
    ASSERT_EQ(units->grid[3][2], NULL_UNIT);
}

TEST_F(units_fixture, units_move_relocates_unit) {
    struct unit unit {
        .x = 2, .y = 3
    };
    units_insert(units, &unit);
    auto index = units->grid[3][2];
    units_move(units, index, 5, 7);
    ASSERT_EQ(units->grid[3][2], NULL_UNIT);
    ASSERT_EQ(units->grid[7][5], index);
}

TEST_F(units_fixture, units_set_enabled_sets_enabled) {
    struct unit unit {
        .x = 3, .y = 5, .player = 2, .enabled = false
    };
    units_insert(units, &unit);
    unit.y = 7;
    units_insert(units, &unit);
    units_set_enabled(units, 2, true);
    ASSERT_TRUE(units->data[units->grid[5][3]].enabled);
    ASSERT_TRUE(units->data[units->grid[7][3]].enabled);
}

TEST_F(units_fixture, units_delete_player_deletes_all_player_units) {
    struct unit unit {
        .x = 3, .y = 5, .player = 2
    };
    units_insert(units, &unit);
    unit.y = 7;
    units_insert(units, &unit);
    units_delete_player(units, 2);
    ASSERT_EQ(units->size, 0);
}

TEST(units_test, merge_health_returns_added_health) {
    struct unit source {
        .health = 2
    };
    struct unit target {
        .health = 3
    };
    ASSERT_EQ(units_merge_health(&source, &target), 5);
}

TEST(units_test, merge_health_bounds_overflow) {
    struct unit source {
        .health = HEALTH_MAX
    };
    struct unit target {
        .health = 1
    };
    ASSERT_EQ(units_merge_health(&source, &target), HEALTH_MAX);
}
