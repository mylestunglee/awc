#define expose_grid_internals
#include "../bitarray.h"
#include "../grid.h"
#include "game_fixture.hpp"

class grid_fixture : public ::testing::Test {
protected:
    grid_fixture() {
        grid_clear_uint8(map);
        clear_energies(energies);
        grid_clear_territory(territory);
    }
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

TEST_F(grid_fixture, grid_correct_territory_santitises_invalid_players) {
    ASSERT_LT(players_capacity, 101);
    territory[2][3] = 101;
    grid_correct_territory(territory, map);
    ASSERT_EQ(territory[2][3], null_player);
}

TEST_F(grid_fixture, grid_correct_territory_santitises_invalid_tiles) {
    ASSERT_LT(tile_capacity, 101);
    map[2][3] = 101;
    grid_correct_territory(territory, map);
    ASSERT_EQ(map[2][3], tile_void);
}

TEST_F(grid_fixture, grid_correct_territory_santitises_invalid_hqs) {
    ASSERT_LT(players_capacity, 101);
    map[2][3] = tile_hq;
    territory[2][3] = 101;
    grid_correct_territory(territory, map);
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

TEST_F(
    game_fixture,
    grid_explore_mark_attackable_tile_marks_when_label_attackable_tiles_flagged) {
    grid_explore_mark_attackable_tile(game, 2, 3, 5, 7, true);
    ASSERT_EQ(game->labels[3][2], attackable_bit);
}

TEST_F(game_fixture,
       grid_explore_mark_attackable_tile_marks_when_damagable_enemy_unit) {
    ASSERT_GE(players_capacity, 2);
    constexpr model_t infantry = 0;
    ASSERT_NE(units_damage[infantry][infantry], 0);
    insert_unit({.player = 1, .x = 2, .y = 3});
    grid_explore_mark_attackable_tile(game, 2, 3, infantry, 0, false);
    ASSERT_EQ(game->labels[3][2], attackable_bit);
}

TEST_F(game_fixture,
       grid_explore_mark_attackable_tile_unmarked_when_undamagable_enemy_unit) {
    ASSERT_GE(players_capacity, 2);
    constexpr model_t infantry = 0;
    constexpr model_t missles = 8;
    ASSERT_EQ(units_damage[missles][infantry], 0);
    insert_unit({.player = 1, .x = 2, .y = 3});
    grid_explore_mark_attackable_tile(game, 2, 3, missles, 0, false);
    ASSERT_EQ(game->labels[3][2], 0);
}

TEST_F(game_fixture,
       grid_explore_mark_attackable_tile_unmarked_when_friendly_unit) {
    ASSERT_GE(players_capacity, 2);
    constexpr model_t infantry = 0;
    ASSERT_NE(units_damage[infantry][infantry], 0);
    insert_unit({.player = 1, .x = 2, .y = 3});
    bitmatrix_set(game->alliances, 0, 1);
    grid_explore_mark_attackable_tile(game, 2, 3, infantry, 0, false);
    ASSERT_EQ(game->labels[3][2], 0);
}

TEST_F(game_fixture, grid_explore_mark_attackable_tile_unmarked_when_no_unit) {
    grid_explore_mark_attackable_tile(game, 2, 3, 5, 0, false);
    ASSERT_EQ(game->labels[3][2], 0);
}

TEST_F(
    game_fixture,
    grid_explore_mark_attackable_direct_marks_adjacent_tiles_with_direct_unit) {
    ASSERT_GE(players_capacity, 2);
    constexpr model_t infantry = 0;
    ASSERT_EQ(models_min_range[infantry], 0);
    grid_explore_mark_attackable_direct(game, 2, 3, infantry, 7, true);
    ASSERT_EQ(game->labels[3][3], attackable_bit);
    ASSERT_EQ(game->labels[3][1], attackable_bit);
    ASSERT_EQ(game->labels[4][2], attackable_bit);
    ASSERT_EQ(game->labels[2][2], attackable_bit);
}

TEST_F(game_fixture,
       grid_explore_mark_attackable_direct_unmarked_with_indirect_unit) {
    ASSERT_GE(players_capacity, 2);
    constexpr model_t artillery = 5;
    ASSERT_GT(models_min_range[artillery], 0);
    grid_explore_mark_attackable_direct(game, 2, 3, artillery, 7, true);
    ASSERT_EQ(game->labels[3][3], 0);
}

TEST_F(game_fixture,
       grid_explore_mark_attackable_ranged_marks_with_correct_range) {
    constexpr model_t artillery = 5;
    ASSERT_GT(models_min_range[artillery], 0);
    grid_explore_mark_attackable_ranged(game, 2, 10, artillery, 5, true);
    ASSERT_EQ(game->labels[10][2], 0);
    ASSERT_EQ(game->labels[11][2], 0);
    ASSERT_EQ(game->labels[12][2], attackable_bit);
    ASSERT_EQ(game->labels[13][2], attackable_bit);
    ASSERT_EQ(game->labels[14][2], 0);
}

TEST_F(game_fixture,
       grid_explore_mark_attackable_ranged_unmarked_with_direct_unit) {
    constexpr model_t infantry = 0;
    ASSERT_EQ(models_min_range[infantry], 0);
    grid_explore_mark_attackable_ranged(game, 2, 3, infantry, 5, true);
    for (auto x = 0; x < grid_size; ++x)
        for (auto y = 0; y < grid_size; ++y)
            ASSERT_EQ(game->labels[y][x], 0);
}

TEST_F(game_fixture,
       is_node_unexplorable_returns_true_when_blocked_by_enemy_unit) {
    insert_unit({.x = 0, .y = 0});
    insert_unit({.player = 1, .x = 2, .y = 3});
    struct list_node node = {.x = 2, .y = 3};
    auto unexplorable = is_node_unexplorable(game, &node, 0);
    ASSERT_TRUE(unexplorable);
}

TEST_F(game_fixture,
       is_node_unexplorable_returns_false_when_blocked_by_passable_unit) {
    constexpr model_t infantry = 0;
    constexpr model_t helicopter = 9;
    insert_unit({.model = infantry, .x = 0, .y = 0});
    insert_unit({.model = helicopter, .player = 1, .x = 2, .y = 3});
    struct list_node node = {.x = 2, .y = 3};
    auto unexplorable = is_node_unexplorable(game, &node, 0);
    ASSERT_FALSE(unexplorable);
}

TEST_F(game_fixture, is_node_unexplorable_returns_true_when_visited) {
    insert_unit({.x = 0, .y = 0});
    game->energies[3][2] = 7;
    struct list_node node = {.x = 2, .y = 3, .energy = 5};
    auto unexplorable = is_node_unexplorable(game, &node, 0);
    ASSERT_TRUE(unexplorable);
}

TEST_F(game_fixture, is_node_unexplorable_returns_false_when_explorable) {
    insert_unit({.x = 0, .y = 0});
    game->map[3][2] = tile_plains;
    struct list_node node = {.x = 2, .y = 3};
    auto unexplorable = is_node_unexplorable(game, &node, 0);
    ASSERT_FALSE(unexplorable);
}

TEST_F(game_fixture, is_node_accessible_returns_true_when_unoccupied) {
    game->x = 2;
    game->y = 3;
    insert_unit({.x = 2, .y = 3});
    struct list_node node = {.x = 5, .y = 7};
    auto accessible = is_node_accessible(game, &node);
    ASSERT_TRUE(accessible);
}

TEST_F(game_fixture, is_node_accessible_returns_true_when_mergable) {
    game->x = 2;
    game->y = 3;
    insert_unit({.x = 2, .y = 3});
    insert_unit({.x = 5, .y = 7});
    struct list_node node = {.x = 5, .y = 7};
    auto accessible = is_node_accessible(game, &node);
    ASSERT_TRUE(accessible);
}

TEST_F(game_fixture, is_node_accessible_returns_false_when_tile_is_occuiped) {
    insert_unit({.health = health_max, .x = 2, .y = 3});
    insert_unit({.health = health_max, .x = 0, .y = 0});
    struct list_node node = {.x = 2, .y = 3};
    auto accessible = is_node_accessible(game, &node);
    ASSERT_FALSE(accessible);
}

TEST_F(game_fixture, is_node_accessible_returns_false_when_ship_on_bridge) {
    ASSERT_NE(movement_type_ship, 0);
    constexpr model_t submarine = 12;
    ASSERT_EQ(unit_movement_types[submarine], movement_type_ship);
    game->map[3][2] = tile_bridge;
    insert_unit({.model = submarine, .x = 0, .y = 0});
    struct list_node node = {.x = 2, .y = 3};
    auto accessible = is_node_accessible(game, &node);
    ASSERT_FALSE(accessible);
}

TEST_F(game_fixture, explore_adjacent_tiles_explores_adjacent_tiles) {
    ASSERT_EQ(movement_type_cost[0][tile_plains], 1);

    bool north_explored = false;
    bool east_explored = false;
    bool south_explored = false;
    bool west_explored = false;

    game->map[2][2] = tile_plains;
    game->map[3][3] = tile_plains;
    game->map[4][2] = tile_plains;
    game->map[3][1] = tile_plains;

    struct list_node node = {.x = 2, .y = 3, .energy = 5};
    explore_adjacent_tiles(game, &node, 0);

    while (!list_empty(&game->list)) {
        node = list_front_pop(&game->list);
        ASSERT_EQ(node.energy, 4);
        if (node.x == 2 && node.y == 2)
            north_explored = true;
        else if (node.x == 3 && node.y == 3)
            east_explored = true;
        else if (node.x == 2 && node.y == 4)
            south_explored = true;
        else if (node.x == 1 && node.y == 3)
            west_explored = true;
    }

    ASSERT_TRUE(north_explored);
    ASSERT_TRUE(east_explored);
    ASSERT_TRUE(south_explored);
    ASSERT_TRUE(west_explored);
}

TEST_F(game_fixture,
       explore_adjacent_tiles_does_not_explore_unaccessible_tiles) {
    ASSERT_EQ(movement_type_cost[0][tile_void], 0);
    struct list_node node = {.x = 2, .y = 3, .energy = 5};
    explore_adjacent_tiles(game, &node, 0);
    ASSERT_TRUE(list_empty(&game->list));
}

TEST_F(game_fixture,
       explore_adjacent_tiles_does_not_explore_tiles_with_depleted_energy) {
    ASSERT_EQ(movement_type_cost[0][tile_plains], 1);

    game->map[2][2] = tile_plains;
    game->map[3][3] = tile_plains;
    game->map[4][2] = tile_plains;
    game->map[3][1] = tile_plains;

    struct list_node node = {.x = 2, .y = 3, .energy = 1};
    explore_adjacent_tiles(game, &node, 0);
    ASSERT_TRUE(list_empty(&game->list));
}

TEST_F(game_fixture, explore_node_does_not_explore_occupied_tile) {
    insert_unit({.x = 0, .y = 0});
    insert_unit({.player = 1, .x = 2, .y = 3});
    struct list_node node = {.x = 2, .y = 3, .energy = 5};
    explore_node(game, &node, null_player, 0, false);
    ASSERT_EQ(game->energies[3][2], 0);
}

TEST_F(game_fixture, explore_node_sets_energies) {
    game->x = 2;
    game->y = 3;
    insert_unit({.x = 2, .y = 3});
    struct list_node node = {.x = 5, .y = 7, .energy = 11};
    explore_node(game, &node, null_player, 0, false);
    ASSERT_EQ(game->energies[7][5], 11);
}

TEST_F(game_fixture, explore_node_sets_attackable_label_if_accessible) {
    game->x = 2;
    game->y = 3;
    insert_unit({.x = 2, .y = 3});
    game->map[7][5] = tile_plains;
    struct list_node node = {.x = 5, .y = 7, .energy = 11};
    explore_node(game, &node, null_player, 0, true);
    ASSERT_EQ(game->labels[7][6], attackable_bit);
}

TEST_F(game_fixture, explore_node_explores_adjacent_tiles) {
    game->x = 2;
    game->y = 3;
    insert_unit({.x = 2, .y = 3});
    struct list_node node = {.x = 5, .y = 7, .energy = 11};
    game->map[7][6] = tile_plains;
    explore_node(game, &node, null_player, 0, false);
    ASSERT_FALSE(list_empty(&game->list));
}

TEST(game_test, init_exploration_energy_lookups_movement) {
    constexpr model_t infantry = 0;
    ASSERT_EQ(init_exploration_energy(2, infantry), 7);
}

TEST_F(game_fixture, grid_explore_recursive_clears_energy) {
    constexpr model_t infantry = 0;
    insert_unit({.model = infantry, .player = 1, .x = 2, .y = 3});
    game->x = 2;
    game->y = 3;
    game->map[3][3] = tile_plains;
    game->energies[3][3] = 101;
    grid_explore_recursive(game, false, 1);
    ASSERT_EQ(game->energies[3][3], 3);
}

TEST_F(grid_fixture, clear_energies_clears_cell) {
    energies[2][3] = 5;
    clear_energies(energies);
    ASSERT_EQ(energies[2][3], 0);
}

TEST_F(game_fixture,
       grid_explore_recursive_explores_with_correct_residual_energies) {
    game->map[3][3] = tile_plains;
    game->map[3][4] = tile_plains;
    game->map[3][5] = tile_plains;
    game->map[3][6] = tile_plains;
    constexpr model_t infantry = 0;
    insert_unit({.model = infantry, .player = 1, .x = 2, .y = 3});
    game->x = 2;
    game->y = 3;
    grid_explore_recursive(game, false, 1);
    ASSERT_EQ(game->energies[3][2], 4);
    ASSERT_EQ(game->energies[3][3], 3);
    ASSERT_EQ(game->energies[3][4], 2);
    ASSERT_EQ(game->energies[3][5], 1);
    ASSERT_EQ(game->energies[3][6], 0);
}

TEST_F(game_fixture, grid_explore_labels_accessible_tiles) {
    game->map[3][3] = tile_plains;
    game->map[3][4] = tile_plains;
    game->map[3][5] = tile_plains;
    game->map[3][6] = tile_plains;
    constexpr model_t infantry = 0;
    insert_unit({.model = infantry, .player = 1, .x = 2, .y = 3});
    game->x = 2;
    game->y = 3;
    grid_explore(game, false);
    ASSERT_EQ(game->labels[3][2], accessible_bit);
    ASSERT_EQ(game->labels[3][3], accessible_bit);
    ASSERT_EQ(game->labels[3][4], accessible_bit);
    ASSERT_EQ(game->labels[3][5], accessible_bit);
    ASSERT_EQ(game->labels[3][6], 0);
}

TEST_F(game_fixture, grid_explore_labels_actionable_attack) {
    constexpr model_t infantry = 0;
    insert_unit({.model = infantry, .player = 0, .x = 2, .y = 3});
    insert_unit({.model = infantry, .player = 1, .x = 3, .y = 3});
    game->x = 2;
    game->y = 3;
    grid_explore(game, false);
    ASSERT_EQ(game->labels[3][3], attackable_bit);
}

TEST_F(game_fixture, grid_explore_labels_potential_attack) {
    constexpr model_t infantry = 0;
    struct unit unit = {.model = infantry, .player = 0, .x = 2, .y = 3};
    units_insert(&game->units, &unit);
    game->x = 2;
    game->y = 3;
    grid_explore(game, true);
    ASSERT_EQ(game->labels[3][3], attackable_bit);
}

TEST_F(game_fixture, grid_explore_is_blocked_by_units) {
    game->map[3][3] = tile_plains;
    game->map[3][4] = tile_plains;
    constexpr model_t infantry = 0;
    struct unit alice = {.model = infantry, .player = 0, .x = 2, .y = 3};
    units_insert(&game->units, &alice);
    struct unit bob = {.model = infantry, .player = 1, .x = 3, .y = 3};
    units_insert(&game->units, &bob);
    game->x = 2;
    game->y = 3;
    grid_explore(game, false);
    ASSERT_EQ(game->labels[3][4], 0);
}

TEST_F(
    game_fixture,
    grid_explore_starting_tile_is_accessible_when_all_adjacent_tiles_are_blocked) {
    constexpr model_t infantry = 0;
    struct unit unit = {.model = infantry, .player = 0, .x = 2, .y = 3};
    units_insert(&game->units, &unit);
    game->x = 2;
    game->y = 3;
    grid_explore(game, false);
    ASSERT_EQ(game->labels[3][2], accessible_bit);
}

TEST_F(game_fixture, grid_explore_merges_labels) {
    game->map[3][3] = tile_plains;
    constexpr model_t infantry = 0;
    struct unit unit = {.model = infantry, .player = 1, .x = 2, .y = 3};
    units_insert(&game->units, &unit);
    game->x = 2;
    game->y = 3;
    grid_explore(game, true);
    ASSERT_EQ(game->labels[3][2], accessible_bit | attackable_bit);
}

TEST_F(game_fixture, grid_explore_recursive_initial_energy_scales) {
    constexpr model_t infantry = 0;
    struct unit unit = {.model = infantry, .player = 0, .x = 2, .y = 3};
    units_insert(&game->units, &unit);
    game->x = 2;
    game->y = 3;
    grid_explore_recursive(game, false, 5);
    ASSERT_EQ(game->energies[3][2], 16);
}

TEST_F(game_fixture, grid_find_path_visits_once_when_zero_energies) {
    grid_find_path(game, 2, 3);
    list_front_pop(&game->list);
    ASSERT_TRUE(list_empty(&game->list));
}

TEST_F(game_fixture, grid_find_path_reaches_strictly_maximal_energy) {
    game->energies[3][2] = 5;
    game->energies[3][3] = 5;
    grid_find_path(game, 2, 3);
    auto node = list_front_pop(&game->list);
    ASSERT_TRUE(list_empty(&game->list));
    ASSERT_EQ(node.x, 2);
    ASSERT_EQ(node.y, 3);
    ASSERT_EQ(node.energy, 5);
}

TEST_F(game_fixture, grid_file_path_traces_path) {
    game->energies[3][3] = 1;
    game->energies[4][3] = 2;
    game->energies[4][4] = 3;
    grid_find_path(game, 2, 3);
    auto size = 0;
    while (!list_empty(&game->list)) {
        list_front_pop(&game->list);
        ++size;
    }
    ASSERT_EQ(size, 4);
}