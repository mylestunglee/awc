#define EXPOSE_GRID_INTERNALS
#include "../src/bitarray.h"
#include "../src/constants.h"
#include "../src/grid.h"
#include "../src/unit_constants.h"
#include "game_fixture.hpp"
#include "test_constants.hpp"

class grid_fixture : public ::testing::Test {
protected:
    grid_fixture() {
        grid_clear_uint8(map);
        clear_energies(energies);
        grid_clear_territory(territory);
    }
    tile_t map[GRID_SIZE][GRID_SIZE];
    energy_t energies[GRID_SIZE][GRID_SIZE];
    player_t territory[GRID_SIZE][GRID_SIZE];
};

TEST_F(grid_fixture, grid_clear_uint8_t_clears_cell) {
    ASSERT_EQ(typeid(tile_t), typeid(uint8_t));
    map[2][3] = 5;

    grid_clear_uint8(map);

    ASSERT_EQ(map[2][3], 0);
}

TEST_F(game_fixture, grid_clear_labels) {
    game->dirty_labels = true;
    game->labels[2][3] = 5;

    grid_clear_labels(game);

    ASSERT_FALSE(game->dirty_labels);
    ASSERT_EQ(game->labels[2][3], 0);
}

TEST_F(grid_fixture, grid_clear_territory_clears_cell) {
    ASSERT_NE(NULL_PLAYER, 2);
    territory[3][5] = 2;

    grid_clear_territory(territory);

    ASSERT_EQ(territory[3][5], NULL_PLAYER);
}

TEST_F(grid_fixture, grid_clear_player_territory_clears_player_cell) {
    ASSERT_NE(NULL_PLAYER, 2);
    territory[3][5] = 2;

    grid_clear_player_territory(map, territory, 2);

    ASSERT_EQ(territory[3][5], NULL_PLAYER);
}

TEST_F(grid_fixture, grid_clear_player_territory_sets_player_hqs_into_cities) {
    ASSERT_NE(NULL_PLAYER, 2);
    map[3][5] = TILE_HQ;
    territory[3][5] = 2;

    grid_clear_player_territory(map, territory, 2);

    ASSERT_EQ(map[3][5], TILE_CITY);
}

TEST_F(grid_fixture, grid_compute_incomes_computes_incomes) {
    ASSERT_NE(NULL_PLAYER, 2);
    money_t incomes[PLAYERS_CAPACITY] = {0};
    territory[3][5] = 2;
    territory[3][6] = 2;
    territory[3][7] = 2;

    grid_compute_incomes(territory, incomes);

    ASSERT_EQ(incomes[2], 3000);
}

TEST_F(
    game_fixture,
    grid_explore_mark_attackable_tile_marks_when_label_attackable_tiles_flagged) {

    grid_explore_mark_attackable_tile(game, 2, 3, 5, 7, true);

    ASSERT_EQ(game->labels[3][2], ATTACKABLE_BIT);
}

TEST_F(game_fixture,
       grid_explore_mark_attackable_tile_marks_when_damagable_enemy_unit) {
    ASSERT_GE(PLAYERS_CAPACITY, 2);
    ASSERT_NE(model_damages[MODEL_INFANTRY][MODEL_INFANTRY], 0);
    insert_unit({.x = 2, .y = 3, .player = 1});

    grid_explore_mark_attackable_tile(game, 2, 3, MODEL_INFANTRY, 0, false);

    ASSERT_EQ(game->labels[3][2], ATTACKABLE_BIT);
}

TEST_F(game_fixture,
       grid_explore_mark_attackable_tile_unmarked_when_undamagable_enemy_unit) {
    ASSERT_GE(PLAYERS_CAPACITY, 2);
    ASSERT_EQ(model_damages[MODEL_MISSLES][MODEL_INFANTRY], 0);
    insert_unit({.x = 2, .y = 3, .player = 1});

    grid_explore_mark_attackable_tile(game, 2, 3, MODEL_MISSLES, 0, false);

    ASSERT_EQ(game->labels[3][2], 0);
}

TEST_F(game_fixture,
       grid_explore_mark_attackable_tile_unmarked_when_friendly_unit) {
    ASSERT_GE(PLAYERS_CAPACITY, 2);
    ASSERT_NE(model_damages[MODEL_INFANTRY][MODEL_INFANTRY], 0);
    insert_unit({.x = 2, .y = 3, .player = 1});
    bitmatrix_set(game->alliances, 0, 1);

    grid_explore_mark_attackable_tile(game, 2, 3, MODEL_INFANTRY, 0, false);

    ASSERT_EQ(game->labels[3][2], 0);
}

TEST_F(game_fixture, grid_explore_mark_attackable_tile_unmarked_when_no_unit) {
    grid_explore_mark_attackable_tile(game, 2, 3, 5, 0, false);

    ASSERT_EQ(game->labels[3][2], 0);
}

TEST_F(
    game_fixture,
    grid_explore_mark_attackable_direct_marks_adjacent_tiles_with_direct_unit) {
    ASSERT_GE(PLAYERS_CAPACITY, 2);
    ASSERT_TRUE(units_is_direct(MODEL_INFANTRY));

    grid_explore_mark_attackable_direct(game, 2, 3, MODEL_INFANTRY, 7, true);

    ASSERT_EQ(game->labels[3][3], ATTACKABLE_BIT);
    ASSERT_EQ(game->labels[3][1], ATTACKABLE_BIT);
    ASSERT_EQ(game->labels[4][2], ATTACKABLE_BIT);
    ASSERT_EQ(game->labels[2][2], ATTACKABLE_BIT);
}

TEST_F(game_fixture,
       grid_explore_mark_attackable_direct_unmarked_with_indirect_unit) {
    ASSERT_GE(PLAYERS_CAPACITY, 2);
    ASSERT_TRUE(units_is_ranged(MODEL_ARTILLERY));

    grid_explore_mark_attackable_direct(game, 2, 3, MODEL_ARTILLERY, 7, true);

    ASSERT_EQ(game->labels[3][3], 0);
}

TEST_F(game_fixture,
       grid_explore_mark_attackable_ranged_marks_with_correct_range) {
    ASSERT_GT(units_is_ranged(MODEL_ARTILLERY), 0);

    grid_explore_mark_attackable_ranged(game, 2, 10, MODEL_ARTILLERY, 5, true);

    ASSERT_EQ(game->labels[10][2], 0);
    ASSERT_EQ(game->labels[11][2], 0);
    ASSERT_EQ(game->labels[12][2], ATTACKABLE_BIT);
    ASSERT_EQ(game->labels[13][2], ATTACKABLE_BIT);
    ASSERT_EQ(game->labels[14][2], 0);
}

TEST_F(game_fixture,
       grid_explore_mark_attackable_ranged_unmarked_with_direct_unit) {
    ASSERT_TRUE(units_is_direct(MODEL_INFANTRY));

    grid_explore_mark_attackable_ranged(game, 2, 3, MODEL_INFANTRY, 5, true);

    for (auto x = 0; x < GRID_SIZE; ++x)
        for (auto y = 0; y < GRID_SIZE; ++y)
            ASSERT_EQ(game->labels[y][x], 0);
}

TEST_F(game_fixture,
       is_node_unexplorable_returns_true_when_blocked_by_enemy_unit) {
    insert_unit();
    insert_unit({.x = 2, .y = 3, .player = 1});
    struct list_node node = {.x = 2, .y = 3};

    auto unexplorable = is_node_unexplorable(game, &node, 0);

    ASSERT_TRUE(unexplorable);
}

TEST_F(game_fixture,
       is_node_unexplorable_returns_false_when_blocked_by_passable_unit) {
    insert_unit();
    insert_unit({.x = 2, .y = 3, .player = 1, .model = MODEL_HELICOPTER});
    struct list_node node = {.x = 2, .y = 3};

    auto unexplorable = is_node_unexplorable(game, &node, 0);

    ASSERT_FALSE(unexplorable);
}

TEST_F(game_fixture, is_node_unexplorable_returns_true_when_visited) {
    insert_unit();
    game->energies[3][2] = 7;
    struct list_node node = {.energy = 5, .x = 2, .y = 3};

    auto unexplorable = is_node_unexplorable(game, &node, 0);

    ASSERT_TRUE(unexplorable);
}

TEST_F(game_fixture, is_node_unexplorable_returns_false_when_explorable) {
    insert_unit();
    game->map[3][2] = TILE_PLAINS;
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
    insert_unit({.health = HEALTH_MAX});
    insert_unit({.x = 2, .y = 3, .health = HEALTH_MAX});
    struct list_node node = {.x = 2, .y = 3};

    auto accessible = is_node_accessible(game, &node);

    ASSERT_FALSE(accessible);
}

TEST_F(game_fixture, is_node_accessible_returns_false_when_ship_on_bridge) {
    ASSERT_NE(MOVEMENT_TYPE_SHIP, 0);
    ASSERT_EQ(model_movements[MODEL_SUBMARINE], MOVEMENT_TYPE_SHIP);
    game->map[3][2] = TILE_BRIDGE;
    insert_unit({.model = MODEL_SUBMARINE});
    struct list_node node = {.x = 2, .y = 3};

    auto accessible = is_node_accessible(game, &node);

    ASSERT_FALSE(accessible);
}

TEST_F(game_fixture, explore_adjacent_tiles_explores_adjacent_tiles) {
    ASSERT_EQ(movement_tile_costs[0][TILE_PLAINS], 1);

    bool north_explored = false;
    bool east_explored = false;
    bool south_explored = false;
    bool west_explored = false;

    game->map[2][2] = TILE_PLAINS;
    game->map[3][3] = TILE_PLAINS;
    game->map[4][2] = TILE_PLAINS;
    game->map[3][1] = TILE_PLAINS;

    struct list_node node = {.energy = 5, .x = 2, .y = 3};

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
    ASSERT_EQ(movement_tile_costs[0][TILE_VOID], 0);
    struct list_node node = {.energy = 5, .x = 2, .y = 3};

    explore_adjacent_tiles(game, &node, 0);

    ASSERT_TRUE(list_empty(&game->list));
}

TEST_F(game_fixture,
       explore_adjacent_tiles_does_not_explore_tiles_with_depleted_energy) {
    ASSERT_EQ(movement_tile_costs[0][TILE_PLAINS], 1);

    game->map[2][2] = TILE_PLAINS;
    game->map[3][3] = TILE_PLAINS;
    game->map[4][2] = TILE_PLAINS;
    game->map[3][1] = TILE_PLAINS;

    struct list_node node = {.energy = 1, .x = 2, .y = 3};

    explore_adjacent_tiles(game, &node, 0);

    ASSERT_TRUE(list_empty(&game->list));
}

TEST_F(game_fixture, explore_node_does_not_explore_occupied_tile) {
    insert_unit();
    insert_unit({.x = 2, .y = 3, .player = 1});
    struct list_node node = {.energy = 5, .x = 2, .y = 3};

    explore_node(game, &node, NULL_PLAYER, 0, false);

    ASSERT_EQ(game->energies[3][2], 0);
}

TEST_F(game_fixture, explore_node_sets_energies) {
    game->x = 2;
    game->y = 3;
    insert_unit({.x = 2, .y = 3});
    struct list_node node = {.energy = 11, .x = 5, .y = 7};

    explore_node(game, &node, NULL_PLAYER, 0, false);

    ASSERT_EQ(game->energies[7][5], 11);
}

TEST_F(game_fixture, explore_node_sets_attackable_label_if_accessible) {
    game->x = 2;
    game->y = 3;
    insert_unit({.x = 2, .y = 3});
    game->map[7][5] = TILE_PLAINS;
    struct list_node node = {.energy = 11, .x = 5, .y = 7};

    explore_node(game, &node, NULL_PLAYER, 0, true);

    ASSERT_EQ(game->labels[7][6], ATTACKABLE_BIT);
}

TEST_F(game_fixture, explore_node_explores_adjacent_tiles) {
    game->x = 2;
    game->y = 3;
    insert_unit({.x = 2, .y = 3});
    struct list_node node = {.energy = 11, .x = 5, .y = 7};
    game->map[7][6] = TILE_PLAINS;

    explore_node(game, &node, NULL_PLAYER, 0, false);

    ASSERT_FALSE(list_empty(&game->list));
}

TEST(game_test, init_exploration_energy_lookups_movement) {
    ASSERT_EQ(init_exploration_energy(2, MODEL_INFANTRY), 7);
}

TEST_F(game_fixture, grid_explore_recursive_clears_energy) {
    insert_unit({.x = 2, .y = 3, .player = 1});
    game->x = 2;
    game->y = 3;
    game->map[3][3] = TILE_PLAINS;
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
    game->map[3][3] = TILE_PLAINS;
    game->map[3][4] = TILE_PLAINS;
    game->map[3][5] = TILE_PLAINS;
    game->map[3][6] = TILE_PLAINS;
    insert_unit({.x = 2, .y = 3, .player = 1});
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
    game->map[3][3] = TILE_PLAINS;
    game->map[3][4] = TILE_PLAINS;
    game->map[3][5] = TILE_PLAINS;
    game->map[3][6] = TILE_PLAINS;
    insert_unit({.x = 2, .y = 3, .player = 1});
    game->x = 2;
    game->y = 3;

    grid_explore(game, false);

    ASSERT_EQ(game->labels[3][2], ACCESSIBLE_BIT);
    ASSERT_EQ(game->labels[3][3], ACCESSIBLE_BIT);
    ASSERT_EQ(game->labels[3][4], ACCESSIBLE_BIT);
    ASSERT_EQ(game->labels[3][5], ACCESSIBLE_BIT);
    ASSERT_EQ(game->labels[3][6], 0);
}

TEST_F(game_fixture, grid_explore_labels_actionable_attack) {
    insert_unit({.x = 2, .y = 3});
    insert_unit({.x = 3, .y = 3, .player = 1});
    game->x = 2;
    game->y = 3;

    grid_explore(game, false);

    ASSERT_EQ(game->labels[3][3], ATTACKABLE_BIT);
}

TEST_F(game_fixture, grid_explore_labels_potential_attack) {
    struct unit unit = {.x = 2, .y = 3};
    units_insert(&game->units, &unit);
    game->x = 2;
    game->y = 3;

    grid_explore(game, true);

    ASSERT_EQ(game->labels[3][3], ATTACKABLE_BIT);
}

TEST_F(game_fixture, grid_explore_is_blocked_by_units) {
    game->map[3][3] = TILE_PLAINS;
    game->map[3][4] = TILE_PLAINS;
    struct unit alice = {.x = 2, .y = 3};
    units_insert(&game->units, &alice);
    struct unit bob = {.x = 3, .y = 3, .player = 1};
    units_insert(&game->units, &bob);
    game->x = 2;
    game->y = 3;

    grid_explore(game, false);

    ASSERT_EQ(game->labels[3][4], 0);
}

TEST_F(
    game_fixture,
    grid_explore_starting_tile_is_accessible_when_all_adjacent_tiles_are_blocked) {
    struct unit unit = {.x = 2, .y = 3};
    units_insert(&game->units, &unit);
    game->x = 2;
    game->y = 3;

    grid_explore(game, false);

    ASSERT_EQ(game->labels[3][2], ACCESSIBLE_BIT);
}

TEST_F(game_fixture, grid_explore_merges_labels) {
    game->map[3][3] = TILE_PLAINS;
    struct unit unit = {.x = 2, .y = 3, .player = 1};
    units_insert(&game->units, &unit);
    game->x = 2;
    game->y = 3;

    grid_explore(game, true);

    ASSERT_EQ(game->labels[3][2], ACCESSIBLE_BIT | ATTACKABLE_BIT);
}

TEST_F(game_fixture, grid_explore_recursive_initial_energy_scales) {
    struct unit unit = {.x = 2, .y = 3};
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
