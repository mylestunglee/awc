#ifndef GAME_FIXTURE_HPP
#define GAME_FIXTURE_HPP

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
