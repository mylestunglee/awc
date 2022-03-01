#ifndef GAME_FIXTURE_HPP
#define GAME_FIXTURE_HPP

#include <gtest/gtest.h>
#include "../game.h"
#include <memory>

class game_fixture : public ::testing::Test {
protected:
    game_fixture();
    void insert_unit(const struct unit&);

private:
    std::unique_ptr<struct game> managed_game;

protected:
    struct game* const game;
};

#endif
