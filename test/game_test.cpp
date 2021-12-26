#define expose_game_internals
#include "../game.h"
#include "game_fixture.hpp"
#include <cstdio>
#include <fstream>

TEST_F(game_fixture, game_load_sets_map) {
    using namespace std;
    auto filename = "test_state.txt";
    {
        ofstream file(filename);
        file << "map \"";
    }
    game_load(game, filename);
    remove(filename);
    ASSERT_EQ(game->map[0][0], tile_plains);
}

TEST_F(game_fixture, find_next_unit_returns_first_enabled_unit_while_hovering) {
    insert_unit({.x = 2, .enabled = true});
    insert_unit({.x = 3, .enabled = false});
    insert_unit({.x = 5, .enabled = true});
    game->x = 5;
    auto unit = find_next_unit(game);
    ASSERT_TRUE(unit);
    ASSERT_EQ(unit->x, 2);
}

TEST_F(game_fixture, find_next_unit_returns_null_when_no_more_enabled_units) {
    insert_unit({.x = 2, .enabled = false});
    game->x = 2;
    auto unit = find_next_unit(game);
    ASSERT_FALSE(unit);
}

TEST_F(game_fixture,
       find_next_unit_returns_first_enabled_unit_while_not_hovering) {
    insert_unit({.x = 2, .enabled = false});
    insert_unit({.x = 3, .enabled = true});
    insert_unit({.x = 5, .enabled = false});
    auto unit = find_next_unit(game);
    ASSERT_TRUE(unit);
    ASSERT_EQ(unit->x, 3);
}

TEST_F(game_fixture, find_next_unit_returns_null_when_no_units) {
    ASSERT_FALSE(find_next_unit(game));
}

TEST_F(game_fixture, game_hover_next_unit_returns_true_when_hovering) {
    insert_unit({.x = 2, .enabled = true});
    ASSERT_TRUE(game_hover_next_unit(game));
    ASSERT_EQ(game->x, 2);
}

TEST_F(game_fixture, game_hover_next_unit_returns_false_when_not_hovering) {
    ASSERT_FALSE(game_hover_next_unit(game));
}

TEST_F(game_fixture, game_attackable_returns_true_when_directly_attackable) {
    insert_unit({});
    units_select_at(&game->units, 0, 0);
    game->prev_x = 2;
    game->x = 3;
    game->labels[0][2] = accessible_bit;
    game->labels[0][3] = attackable_bit;
    ASSERT_TRUE(game_attackable(game));
}

TEST_F(game_fixture, game_attackable_returns_true_when_indirectly_attackable) {
    constexpr model_t artillery = 5;
    insert_unit({.model = artillery});
    units_select_at(&game->units, 0, 0);
    game->x = 3;
    game->labels[0][3] = attackable_bit;
    ASSERT_TRUE(game_attackable(game));
}

TEST_F(game_fixture, game_buildable_returns_true_when_buildable) {
    game->x = 2;
    game->y = 3;
    game->turn = 5;
    game->territory[3][2] = 5;
    game->map[3][2] = tile_factory;
    ASSERT_TRUE(game_buildable(game));
}

TEST_F(game_fixture, calc_damage_between_two_infantry) {
    constexpr model_t infantry = 0;
    insert_unit({.health = health_max, .model = infantry, .x = 0});
    insert_unit({.health = health_max, .model = infantry, .x = 1});
    game->map[0][1] = tile_plains;
    ASSERT_EQ(calc_damage(game, units_const_get_at(&game->units, 0, 0),
                          units_const_get_at(&game->units, 1, 0)),
              static_cast<health_t>(55.0 * 0.9 * 255.0 / 100.0));
}

TEST_F(game_fixture, game_simulate_attack_kill_attackee) {
    constexpr model_t infantry = 0;
    insert_unit({.health = health_max, .model = infantry, .x = 2, .y = 3});
    insert_unit({.health = 1, .model = infantry, .x = 5, .y = 7});
    units_select_at(&game->units, 2, 3);
    game->x = 5;
    game->y = 7;
    game->map[7][5] = tile_plains;

    health_t damage = 0;
    health_t counter_damage = 0;
    game_simulate_attack(game, &damage, &counter_damage);

    ASSERT_GT(damage, 0);
    ASSERT_EQ(counter_damage, 0);
}

TEST_F(game_fixture, game_simulate_attack_ranged_units_do_not_counter_attack) {
    constexpr model_t infantry = 0;
    constexpr model_t artillery = 5;
    insert_unit({.health = health_max, .model = artillery, .x = 2, .y = 3});
    insert_unit({.health = health_max, .model = infantry, .x = 5, .y = 7});
    units_select_at(&game->units, 2, 3);
    game->x = 5;
    game->y = 7;
    game->map[7][5] = tile_plains;

    health_t damage = 0;
    health_t counter_damage = 0;
    game_simulate_attack(game, &damage, &counter_damage);

    ASSERT_GT(damage, 0);
    ASSERT_EQ(counter_damage, 0);
}

TEST_F(game_fixture, game_simulate_attack_when_counter_attacking) {
    constexpr model_t infantry = 0;
    insert_unit({.health = health_max, .model = infantry, .x = 2, .y = 3});
    insert_unit({.health = health_max, .model = infantry, .x = 5, .y = 7});
    units_select_at(&game->units, 2, 3);
    game->x = 5;
    game->y = 7;
    game->map[3][2] = tile_plains;
    game->map[7][5] = tile_plains;

    health_t damage = 0;
    health_t counter_damage = 0;
    game_simulate_attack(game, &damage, &counter_damage);

    ASSERT_EQ(damage, calc_damage(game, units_const_get_at(&game->units, 2, 3),
                                  units_const_get_at(&game->units, 5, 7)));
    ASSERT_EQ(counter_damage, (health_max - damage) * damage / health_max);
}