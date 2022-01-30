#ifndef game_fixture_h
#define game_fixture_h

#include <gtest/gtest.h>
#include "../game.h"

class game_fixture : public ::testing::Test {
protected:
    game_fixture();
    ~game_fixture();
    void insert_unit(const struct unit& unit);
    struct game* const game;
};

#endif
