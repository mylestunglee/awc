#define EXPOSE_FILE_INTERNALS
#include "../file.h"
#include "game_fixture.hpp"

TEST(file_test, load_turn_when_invalid_keyword) {
    ASSERT_FALSE(load_turn("", "", nullptr));
}
