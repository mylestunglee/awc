#include "grid.h"

void grid_clear_all(uint8_t grid[grid_size][grid_size]) {
    grid_index y = 0;
    do {
        grid_index x = 0;
        do {
            grid[y][x] = 0;
        } while (++x);
    } while (++y);
}

static void grid_explore_recursive(
    const struct game* const game,
    uint8_t labels[grid_size][grid_size],
    uint8_t workspace[grid_size][grid_size],
    const grid_index x,
    const grid_index y,
    uint8_t energy) {
    if (energy < 1) {
        return;
    }
    energy -= 1;
    if (workspace[y][x] > energy) {
        return;
    }
    workspace[y][x] = energy;

    labels[y][x] |= accessible_bit;
    labels[y][(grid_index)(x + 1)] |= attackable_bit;
    labels[y][(grid_index)(x - 1)] |= attackable_bit;
    labels[(grid_index)(y + 1)][x] |= attackable_bit;
    labels[(grid_index)(y - 1)][x] |= attackable_bit;

    grid_explore_recursive(game, labels, workspace, x + 1, y, energy);
    grid_explore_recursive(game, labels, workspace, x - 1, y, energy);
    grid_explore_recursive(game, labels, workspace, x, y + 1, energy);
    grid_explore_recursive(game, labels, workspace, x, y - 1, energy);
}

void grid_explore(struct game* const game) {
    grid_explore_recursive(
        game,
        game->labels,
        game->workspace,
        game->units.data[game->selected].x,
        game->units.data[game->selected].y, 4);
}

