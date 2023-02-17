#ifndef GAME_FIXTURE_HPP
#define GAME_FIXTURE_HPP

#include "../src/game.h"
#include <gtest/gtest.h>
#include <memory>

class game_fixture : public ::testing::Test {
protected:
    game_fixture();
    struct unit* insert_unit(const struct unit& = {});
    struct unit* insert_selected_unit(const struct unit& = {});

private:
    std::unique_ptr<struct game> managed_game;

protected:
    struct game* const game;
};

#endif
