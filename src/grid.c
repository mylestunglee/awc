#include "grid.h"
#include "bitarray.h"
#include "constants.h"
#include "unit_constants.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void grid_clear_uint8(uint8_t grid[GRID_SIZE][GRID_SIZE]) {
    memset(grid, 0, GRID_SIZE * GRID_SIZE);
}

void grid_clear_labels(struct game* const game) {
    if (game->dirty_labels) {
        grid_clear_uint8(game->labels);
        game->dirty_labels = false;
    }
}

void grid_clear_territory(player_t territory[GRID_SIZE][GRID_SIZE]) {
    assert(sizeof(player_t) == 1);
    memset(territory, NULL_PLAYER, GRID_SIZE * GRID_SIZE);
}

void grid_transfer_player_territory(tile_t map[GRID_SIZE][GRID_SIZE],
                                    player_t territory[GRID_SIZE][GRID_SIZE],
                                    const player_t loser,
                                    const player_t winner) {
    grid_t y = 0;
    do {
        grid_t x = 0;
        do
            if (territory[y][x] == loser) {
                territory[y][x] = winner;
                if (map[y][x] == TILE_HQ)
                    map[y][x] = TILE_CITY;
            }
        while (++x);
    } while (++y);
}

void grid_compute_incomes(player_t territory[GRID_SIZE][GRID_SIZE],
                          money_t incomes[PLAYERS_CAPACITY]) {
    grid_t y = 0;
    do {
        grid_t x = 0;
        do {
            const player_t player = territory[y][x];
            if (player != NULL_PLAYER)
                incomes[player] += MONEY_SCALE;
        } while (++x);
    } while (++y);
}

// Marks a tile as attackable if position relates to attackable unit
void grid_explore_mark_attackable_tile(struct game* const game, const grid_t x,
                                       const grid_t y, const model_t model,
                                       const player_t player,
                                       const bool label_attackable_tiles) {
    if (!label_attackable_tiles) {
        const struct unit* const unit =
            units_const_get_at_safe(&game->units, x, y);
        if (!unit)
            return;
        const bool friendly =
            bitmatrix_get(game->alliances, player, unit->player);
        const bool self = game->x == x && game->y == y;
        const bool undamagable = model_damages[model][unit->model] == 0;
        if (self || friendly || undamagable)
            return;
    }

    game->labels[y][x] |= ATTACKABLE_BIT;
}

void grid_explore_mark_attackable_direct(struct game* const game,
                                         const grid_t x, const grid_t y,
                                         const model_t model,
                                         const player_t player,
                                         const bool label_attackable_tiles) {
    if (units_is_ranged(model))
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

    if (!units_is_ranged(model))
        return;

    const grid_wide_t min_range = model_min_ranges[model];
    const grid_wide_t max_range = model_max_ranges[model];

    for (grid_wide_t j = -max_range; j <= max_range; ++j)
        for (grid_wide_t i = -max_range; i <= max_range; ++i) {
            const grid_wide_t distance = abs(i) + abs(j);
            if (min_range <= distance && distance <= max_range)
                grid_explore_mark_attackable_tile(
                    game, (grid_wide_t)x + i, (grid_wide_t)y + j, model, player,
                    label_attackable_tiles);
        }
}

// player may not equal game->turn
bool is_node_unexplorable(const struct game* const game,
                          const struct list_node* const node,
                          const player_t player) {
    const struct unit* const source =
        units_const_get_at(&game->units, game->x, game->y);
    const struct unit* const target =
        units_const_get_at_safe(&game->units, node->x, node->y);
    if (target) {
        const bool unfriendly =
            !bitmatrix_get(game->alliances, player, target->player);
        const bool noninit_tile = source != target;
        const bool unpassable =
            model_passes[source->model] == model_passes[target->model];

        if (unfriendly && noninit_tile && unpassable)
            return true;
    }

    // Do not re-compute explored areas
    return game->energies[node->y][node->x] > node->energy;
}

bool is_node_accessible(const struct game* const game,
                        const struct list_node* const node) {
    const struct unit* const source =
        units_const_get_at(&game->units, game->x, game->y);
    assert(source);
    const struct unit* const target =
        units_const_get_at_safe(&game->units, node->x, node->y);
    const bool unoccupied = !target;
    const bool init_tile = source == target;
    const tile_t tile = game->map[node->y][node->x];
    // NOLINTNEXTLINE
    const bool ship_on_bridge =
        tile == TILE_BRIDGE &&
        model_movements[source->model] == MOVEMENT_TYPE_SHIP;
    return init_tile ||
           ((unoccupied || units_mergable(source, target)) && !ship_on_bridge);
}

void explore_adjacent_tiles(struct game* const game,
                            const struct list_node* const node,
                            const model_t model) {
    const grid_t x = node->x;
    const grid_t y = node->y;

    const grid_t adjacent_x[] = {(grid_t)(x + 1), x, (grid_t)(x - 1), x};
    const grid_t adjacent_y[] = {y, (grid_t)(y - 1), y, (grid_t)(y + 1)};

    for (uint8_t i = 0; i < 4; ++i) {
        const grid_t x_i = adjacent_x[i];
        const grid_t y_i = adjacent_y[i];
        const tile_t tile = game->map[y_i][x_i];
        const movement_t movement = model_movements[model];
        const energy_t cost = movement_tile_costs[movement][tile];

        if (cost == 0)
            continue;

        if (node->energy <= cost)
            continue;

        struct list_node adjacent_node = {
            .energy = (energy_t)(node->energy - cost), .x = x_i, .y = y_i};

        list_insert(&game->list, &adjacent_node);
    }
}

void explore_node(struct game* const game, const struct list_node* const node,
                  const player_t player, const model_t model,
                  const bool label_attackable_tiles) {
    assert(node->energy > 0);

    if (is_node_unexplorable(game, node, player))
        return;

    game->energies[node->y][node->x] = node->energy;

    if (is_node_accessible(game, node)) {
        game->labels[node->y][node->x] |= ACCESSIBLE_BIT;
        grid_explore_mark_attackable_direct(game, node->x, node->y, model,
                                            player, label_attackable_tiles);
    }

    explore_adjacent_tiles(game, node, model);
}

void clear_energies(energy_t energies[GRID_SIZE][GRID_SIZE]) {
    memset(energies, 0, sizeof(energy_t) * GRID_SIZE * GRID_SIZE);
}

energy_t init_exploration_energy(const energy_t scalar, const model_t model) {
    const movement_t movement_type = model_movements[model];
    return scalar * model_movement_ranges[movement_type] + 1;
}

// Use scalar > 1 when looking ahead multiple turns
void grid_explore_recursive(struct game* const game,
                            const bool label_attackable_tiles,
                            const energy_t scalar) {
    clear_energies(game->energies);
    game->dirty_labels = true;
    struct list* const list = &game->list;

    list_initialise(&game->list);
    assert(units_exists_at(&game->units, game->x, game->y));
    assert(scalar > 0);

    // Use cursor instead of selected property because we want to highlight
    // non-selectable enemy units
    const unit_t cursor_unit_index = game->units.grid[game->y][game->x];
    const struct unit cursor_unit = game->units.data[cursor_unit_index];

    const model_t model = cursor_unit.model;
    const player_t player = cursor_unit.player;

    grid_explore_mark_attackable_ranged(game, game->x, game->y, model, player,
                                        label_attackable_tiles);

    {
        struct list_node node = {.energy =
                                     init_exploration_energy(scalar, model),
                                 .x = game->x,
                                 .y = game->y};
        list_insert(list, &node);
    }

    while (!list_empty(list)) {
        const struct list_node node = list_front_pop(list);
        explore_node(game, &node, player, model, label_attackable_tiles);
    }
}

// Recursively marks tiles that are accessible or attackable from the cursor
// tile
void grid_explore(struct game* const game, const bool label_attackable_tiles) {
    grid_explore_recursive(game, label_attackable_tiles, 1);
}

// Populates game.list with coordinates along the path to maximal energy
void grid_find_path(struct game* const game, grid_t x, grid_t y) {
    list_initialise(&game->list);

    energy_t prev_energy = game->energies[y][x];

    while (true) {
        struct list_node node = {
            .energy = game->energies[y][x], .x = x, .y = y};
        list_insert(&game->list, &node);

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

        if (next_energy <= prev_energy)
            return;

        prev_energy = next_energy;
    }
}
