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

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}