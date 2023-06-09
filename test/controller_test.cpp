#include "../src/controller.h"
#include "game_fixture.hpp"

TEST_F(game_fixture, controller_run_quit) {
    static unsigned count = 0;

    controller_run(game, [] {
        ++count;
        return 'q';
    });

    ASSERT_EQ(count, 1);
}

TEST_F(game_fixture, controller_run_pan_right) {
    static unsigned count = 0;

    controller_run(game, [] {
        ++count;
        return count == 1 ? 'd' : 'q';
    });

    ASSERT_EQ(game->x, 1);
    ASSERT_EQ(count, 2);
}
