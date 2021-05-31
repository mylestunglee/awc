#include "grid.h"
#include "bitarray.h"
#include "definitions.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void grid_clear_uint8(uint8_t grid[grid_size][grid_size]) {
    memset(grid, 0, grid_size * grid_size);
}

void grid_clear_energy(energy_t energies[grid_size][grid_size]) {
    memset(energies, 0, sizeof(energy_t) * grid_size * grid_size);
}

void grid_clear_territory(player_t territory[grid_size][grid_size]) {
    grid_t y = 0;
    do {
        grid_t x = 0;
        do
            territory[y][x] = null_player;
        while (++x);
    } while (++y);
}

void grid_clear_player_territory(tile_t map[grid_size][grid_size],
                                 player_t territory[grid_size][grid_size],
                                 const player_t player) {

    grid_t y = 0;
    do {
        grid_t x = 0;
        do
            if (territory[y][x] == player) {
                territory[y][x] = null_player;
                if (map[y][x] == tile_hq)
                    map[y][x] = tile_city;
            }
        while (++x);
    } while (++y);
}

// Normalise invalid map territory state
void grid_correct(player_t territory[grid_size][grid_size],
                  tile_t map[grid_size][grid_size]) {

    grid_t y = 0;
    do {
        grid_t x = 0;
        do {
            if (territory[y][x] > players_capacity)
                territory[y][x] = null_player;
            if (map[y][x] > tile_capacity)
                map[y][x] = tile_void;
        } while (++x);
    } while (++y);

    grid_clear_player_territory(map, territory, null_player);
}

void grid_compute_incomes(player_t territory[grid_size][grid_size],
                          gold_t incomes[players_capacity]) {
    grid_t y = 0;
    do {
        grid_t x = 0;
        do {
            const player_t player = territory[y][x];
            if (player != null_player)
                ++incomes[player];
        } while (++x);
    } while (++y);
}

// Marks a tile as attackable if position relates to attackable unit
void grid_explore_mark_attackable_tile(struct game* const game, const grid_t x,
                                       const grid_t y, const model_t model,
                                       const player_t player,
                                       const bool label_attackable_tiles) {

    // By-pass checks when label attackable tiles forcefully
    if (label_attackable_tiles) {
        game->labels[y][x] |= attackable_bit;
        return;
    }

    const unit_t index = game->units.grid[y][x];
    if (index != null_unit)
        return;

    const struct unit* const unit = &game->units.data[index];

    // Mark tiles without friendly units as attackable
    if (!bitmatrix_get(game->alliances, unit->player, player) &&
        units_damage[model][unit->model])
        game->labels[y][x] |= attackable_bit;
}

void grid_explore_mark_attackable_direct(struct game* const game,
                                         const grid_t x, const grid_t y,
                                         const model_t model,
                                         const player_t player,
                                         const bool label_attackable_tiles) {

    // Direct attack only applies to 0-ranged models
    if (models_min_range[model])
        return;

    grid_explore_mark_attackable_tile(game, x + 1, y, model, player,
                                      label_attackable_tiles);
    grid_explore_mark_attackable_tile(game, x - 1, y, model, player,
                                      label_attackable_tiles);
    grid_explore_mark_attackable_tile(game, x, y + 1, model, player,
                                      label_attackable_tiles);
    grid_explore_mark_attackable_tile(game, x, y - 1, model, player,
                                      label_attackable_tiles);
}

void grid_explore_mark_attackable_ranged(struct game* const game,
                                         const grid_t x, const grid_t y,
                                         const model_t model,
                                         const player_t player,
                                         const bool label_attackable_tiles) {

    const grid_wide_t min_range = models_min_range[model];
    const grid_wide_t max_range = models_max_range[model];

    // Range attack only applies to positive ranged models
    if (min_range == 0)
        return;

    for (grid_wide_t j = -max_range; j <= max_range; ++j)
        for (grid_wide_t i = -max_range; i <= max_range; ++i) {
            const grid_wide_t distance = abs(i) + abs(j);
            if (min_range <= distance && distance <= max_range)
                grid_explore_mark_attackable_tile(
                    game, x + i, y + j, model, player, label_attackable_tiles);
        }
}

// Recursively marks tiles that are accessible or attackable from the cursor
// tile
void grid_explore(struct game* const game, const bool label_attackable_tiles,
                  const bool friendly_passable) {
    grid_explore_recursive(game, label_attackable_tiles, friendly_passable, 1);
}

energy_t explore_node_deplete_energy(struct game* const game,
                         const struct list_node* const node,
                         const player_t player,
                         const model_t model) {
    const movement_t movement = unit_movement_types[model];
    const tile_t tile = game->map[node->y][node->x];
    const energy_t cost = movement_type_cost[movement][tile];

    // Inaccessible terrian
    if (cost == 0)
        return 0;

    const energy_t energy = node->energy > cost ? node->energy - cost : 0;
    // Not enough energy to keep moving
    if (energy == 0)
        return 0;

    // If friendly_passable, then cannot pass through enemy units
    // If not friendly_passable, then cannot pass through non-self units
    // cursor_unit->player may not be equals to game->turn because the inspected
    // unit may not be ours
    const unit_t node_unit_index = game->units.grid[node->y][node->x];
    const player_t node_unit_player = game->units.data[node_unit_index].player;
    const bool friendly_node_unit =
        bitmatrix_get(game->alliances, player, node_unit_player);

    if (node_unit_index != null_unit && !friendly_node_unit)
        return 0;

    // Do not re-compute explored areas
    if (game->energies[node->y][node->x] > energy)
        return 0;

    return energy;
}

void explore_node(struct game* const game,
                         const struct list_node* const node,
                         const player_t player,
                         const model_t model,
                         const bool label_attackable_tiles) {

    const energy_t energy = explore_node_deplete_energy(
        game, node, player, model);

    if (energy == 0)
        return;

    game->energies[node->y][node->x] = node->energy;

    const unit_t node_unit_index = game->units.grid[node->y][node->x];
    const tile_t tile = game->map[node->y][node->x];
    const movement_t movement = unit_movement_types[model];

    // Mark unit-free tiles as accessible but ships cannot block bridges
    if (node_unit_index == null_unit &&
        !(tile == tile_bridge && movement == movement_type_ship)) {

        game->labels[node->y][node->x] |= accessible_bit;
        grid_explore_mark_attackable_direct(
            game, node->x, node->y, model, player,
            label_attackable_tiles);
    }

    // Explore adjacent tiles
    struct list* const list = &game->list;

    struct list_node east = {
        .x = (grid_t)(node->x + 1), .y = node->y, .energy = energy};
    struct list_node west = {
        .x = (grid_t)(node->x - 1), .y = node->y, .energy = energy};
    struct list_node south = {
        .x = node->x, .y = (grid_t)(node->y + 1), .energy = energy};
    struct list_node north = {
        .x = node->x, .y = (grid_t)(node->y - 1), .energy = energy};

    list_insert(list, east);
    list_insert(list, west);
    list_insert(list, south);
    list_insert(list, north);
}

// Use scalar > 1 when looking ahead multiple turns
void grid_explore_recursive(struct game* const game,
                            const bool label_attackable_tiles,
                            const bool friendly_passable,
                            const energy_t scalar) {
    struct list* const list = &game->list;

    assert(list_empty(list));
    assert(game->units.grid[game->y][game->x] != null_unit);
    assert(scalar > 0);

    // Use cursor instead of selected property because we want to highlight
    // non-selectable enemy units
    const unit_t cursor_unit_index = game->units.grid[game->y][game->x];
    const struct unit cursor_unit = game->units.data[cursor_unit_index];
    // pop unit from units so unit cannot interfere with its own pathfinding
    units_delete(&game->units, cursor_unit_index);
    assert(game->units.grid[game->y][game->x] == null_unit);

    const model_t model = cursor_unit.model;
    const player_t player = cursor_unit.player;

    const movement_t movement_type = unit_movement_types[model];
    const energy_t init_energy =
        scalar * unit_movement_ranges[movement_type] + 1;

    grid_explore_mark_attackable_ranged(game, game->x, game->y,
                                        model, player,
                                        label_attackable_tiles);

    struct list_node node = {.x = game->x, .y = game->y, .energy = init_energy};

    list_insert(list, node);

    while (!list_empty(list)) {
        const struct list_node node = list_front_pop(list);
        explore_node(game, &node,
            friendly_passable ? player : null_player,
            model, label_attackable_tiles);
    }

    units_insert(&game->units, cursor_unit);
}

// Populates game.list with coordinates along the path to maximal energy
void grid_find_path(struct game* const game, grid_t x, grid_t y) {
    assert(list_empty(&game->list));

    energy_t prev_energy = 0;

    while (true) {
        struct list_node list_node = {
            .x = x, .y = y, .energy = game->energies[y][x]};
        list_insert(&game->list, list_node);

        energy_t next_energy = 0;

        const grid_t adjacent_x[] = {(grid_t)(x + 1), x, (grid_t)(x - 1), x};
        const grid_t adjacent_y[] = {y, (grid_t)(y - 1), y, (grid_t)(y + 1)};

        for (uint8_t i = 0; i < 4; ++i) {
            energy_t energy = game->energies[adjacent_y[i]][adjacent_x[i]];

            if (energy > next_energy) {
                x = adjacent_x[i];
                y = adjacent_y[i];
                next_energy = energy;
            }
        }

        if (next_energy < prev_energy)
            return;

        prev_energy = next_energy;
    }
}
