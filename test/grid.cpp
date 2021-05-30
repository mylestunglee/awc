#include "game_fixture.hpp"

class grid_fixture : public ::testing::Test {
protected:
    tile_t map[grid_size][grid_size];
    energy_t energies[grid_size][grid_size];
    player_t territory[grid_size][grid_size];
};

TEST_F(grid_fixture, grid_clear_uint8_t_clears_cell) {
    ASSERT_EQ(typeid(tile_t), typeid(uint8_t));
    map[2][3] = 5;
    grid_clear_uint8(map);
    ASSERT_EQ(map[2][3], 0);
}

TEST_F(grid_fixture, grid_clear_energy_clears_cell) {
    energies[2][3] = 5;
    grid_clear_energy(energies);
    ASSERT_EQ(energies[2][3], 0);
}

TEST_F(grid_fixture, grid_clear_territory_clears_cell) {
    ASSERT_NE(null_player, 2);
    territory[3][5] = 2;
    grid_clear_territory(territory);
    ASSERT_EQ(territory[3][5], null_player);
}

TEST_F(grid_fixture, grid_clear_player_territory_clears_player_cell) {
    ASSERT_NE(null_player, 2);
    territory[3][5] = 2;
    grid_clear_player_territory(map, territory, 2);
    ASSERT_EQ(territory[3][5], null_player);
}

TEST_F(grid_fixture, grid_clear_player_territory_sets_player_hqs_into_cities) {
    ASSERT_NE(null_player, 2);
    map[3][5] = tile_hq;
    territory[3][5] = 2;
    grid_clear_player_territory(map, territory, 2);
    ASSERT_EQ(map[3][5], tile_city);
}

TEST_F(grid_fixture, grid_correct_santitises_invalid_players) {
    ASSERT_LT(players_capacity, 101);
    territory[2][3] = 101;
    grid_correct(territory, map);
    ASSERT_EQ(territory[2][3], null_player);
}

TEST_F(grid_fixture, grid_correct_santitises_invalid_tiles) {
    ASSERT_LT(tile_capacity, 101);
    map[2][3] = 101;
    grid_correct(territory, map);
    ASSERT_EQ(map[2][3], tile_void);
}

TEST_F(grid_fixture, grid_correct_santitises_invalid_hqs) {
    ASSERT_LT(players_capacity, 101);
    map[2][3] = tile_hq;
    territory[2][3] = 101;
    grid_correct(territory, map);
    ASSERT_EQ(map[2][3], tile_city);
    ASSERT_EQ(territory[2][3], null_player);
}

TEST_F(grid_fixture, grid_compute_incomes_computes_incomes) {
    ASSERT_NE(null_player, 2);
    gold_t incomes[players_capacity] = {0};
    territory[3][5] = 2;
    territory[3][6] = 2;
    territory[3][7] = 2;
    grid_compute_incomes(territory, incomes);
    ASSERT_EQ(incomes[2], 3);
}

TEST_F(game_fixture, grid_explore_mark_attackable_tile_marks_when_label_attackable_tiles_flagged) {
    grid_explore_mark_attackable_tile(game, 2, 3, 5, 7, true);
    ASSERT_EQ(game->labels[3][2], attackable_bit);
}

TEST_F(game_fixture, grid_explore_mark_attackable_tile_marks_when_damagable_enemy_unit) {
    ASSERT_GE(players_capacity, 2);
    constexpr model_t infantry = 0;
    ASSERT_NE(units_damage[infantry][infantry], 0);
    struct unit enemy_unit = {.player = 1, .x = 3, .y = 5};
    units_insert(&game->units, enemy_unit);
    grid_explore_mark_attackable_tile(game, 2, 3, infantry, 0, true);
    ASSERT_EQ(game->labels[3][2], attackable_bit);
}

TEST_F(game_fixture, grid_explore_mark_attackable_tile_unmarked_when_undamagable_enemy_unit) {
    ASSERT_GE(players_capacity, 2);
    constexpr model_t infantry = 0;
    constexpr model_t missles = 9;
    ASSERT_NE(units_damage[missles][infantry], 0);
    struct unit enemy_unit = {.player = 1, .x = 3, .y = 5};
    units_insert(&game->units, enemy_unit);
    grid_explore_mark_attackable_tile(game, 2, 3, missles, 0, true);
    ASSERT_EQ(game->labels[3][5], 0);
}

TEST_F(game_fixture, grid_explore_mark_attackable_tile_unmarked_when_friendly_unit) {
    ASSERT_GE(players_capacity, 2);
    constexpr model_t infantry = 0;
    ASSERT_NE(units_damage[infantry][infantry], 0);
    struct unit enemy_unit = {.player = 1, .x = 3, .y = 5};
    units_insert(&game->units, enemy_unit);
    bitmatrix_set(game->alliances, 0, 1);
    grid_explore_mark_attackable_tile(game, 2, 3, infantry, 0, true);
    ASSERT_EQ(game->labels[3][5], 0);
}

TEST_F(game_fixture, grid_explore_mark_attackable_direct_marks_adjacent_tiles_with_direct_unit) {
    ASSERT_GE(players_capacity, 2);
    constexpr model_t infantry = 0;
    ASSERT_EQ(models_min_range[infantry], 0);
    grid_explore_mark_attackable_direct(game, 2, 3, infantry, 7, true);
    ASSERT_EQ(game->labels[3][3], attackable_bit);
    ASSERT_EQ(game->labels[3][1], attackable_bit);
    ASSERT_EQ(game->labels[4][2], attackable_bit);
    ASSERT_EQ(game->labels[2][2], attackable_bit);
}

TEST_F(game_fixture, grid_explore_mark_attackable_direct_unmarked_with_indirect_unit) {
    ASSERT_GE(players_capacity, 2);
    constexpr model_t artillery = 5;
    ASSERT_GT(models_min_range[artillery], 0);
    grid_explore_mark_attackable_direct(game, 2, 3, artillery, 7, true);
    ASSERT_EQ(game->labels[3][3], 0);
}

TEST_F(game_fixture, grid_explore_mark_attackable_ranged_marks_with_correct_range) {
    constexpr model_t artillery = 5;
    ASSERT_GT(models_min_range[artillery], 0);
    grid_explore_mark_attackable_ranged(game, 2, 10, artillery, 5, true);
    ASSERT_EQ(game->labels[10][2], 0);
    ASSERT_EQ(game->labels[11][2], 0);
    ASSERT_EQ(game->labels[12][2], attackable_bit);
    ASSERT_EQ(game->labels[13][2], attackable_bit);
    ASSERT_EQ(game->labels[14][2], 0);
}

TEST_F(game_fixture, grid_explore_mark_attackable_ranged_unmarked_with_direct_unit) {
    constexpr model_t infantry = 0;
    ASSERT_EQ(models_min_range[infantry], 0);
    grid_explore_mark_attackable_ranged(game, 2, 3, infantry, 5, true);
    for (auto x = 0; x < grid_size; ++x)
        for (auto y = 0; y < grid_size; ++y)
            ASSERT_EQ(game->labels[y][x], 0);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}