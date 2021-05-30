#include <gtest/gtest.h>
#include <glpk.h>
#include "../units.c"
#include "../game.c"
#include "../file.c"
#include "../action.c"
#include "../graphics.c"
#include "../grid.c"
#include "../list.c"
#include "../bitarray.c"
#include "../console.c"
#include "../bot.c"
#include "../optimise.c"

class game_fixture : public ::testing::Test {
protected:
    game_fixture() : game (new struct game) {
        game_preload(game);
    }
    ~game_fixture() {
        delete game;
    }
    struct game* const game;
};
