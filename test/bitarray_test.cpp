#include "../src/constants.h"
#include <gtest/gtest.h>
#define EXPOSE_UNITS_INTERNALS
#include "../src/bitarray.h"

TEST(bitarray, bitarray_set) {
    uint8_t data = '\x00';

    bitarray_set(&data, 2);

    ASSERT_EQ(data, '\x04');
}

TEST(bitarray, bitarray_unset) {
    uint8_t data = '\xff';

    bitarray_unset(&data, 2);

    ASSERT_EQ(data, '\xfb');
}

TEST(bitarray, bitmatrix_cascade_indexes_first_bit) {
    ASSERT_EQ(bitmatrix_cascade(1, 0), 0);
}

TEST(bitarray, bitmatrix_cascade_indexes_first_row) {
    ASSERT_EQ(bitmatrix_cascade(2, 1), PLAYERS_CAPACITY - 1);
}

TEST(bitarray, bitmatrix_set_when_null_player) {
    uint8_t data = '\x00';

    bitmatrix_set(&data, NULL_PLAYER, 0);

    ASSERT_EQ(data, '\x00');
}

TEST(bitarray, bitmatrix_set_when_same_players) {
    uint8_t data = '\x00';

    bitmatrix_set(&data, 2, 2);

    ASSERT_EQ(data, '\x00');
}

TEST(bitarray, bitmatrix_set_when_unflipped_cascade) {
    uint8_t data = '\x00';

    bitmatrix_set(&data, 2, 0);

    ASSERT_EQ(data, '\x02');
}

TEST(bitarray, bitmatrix_set_when_flipped_cascade) {
    uint8_t data = '\x00';

    bitmatrix_set(&data, 0, 2);

    ASSERT_EQ(data, '\x02');
}

TEST(bitarray, bitmatrix_unset_when_null_player) {
    uint8_t data = '\xff';

    bitmatrix_unset(&data, NULL_PLAYER, 0);

    ASSERT_EQ(data, '\xff');
}

TEST(bitarray, bitmatrix_unset_when_same_players) {
    uint8_t data = '\xff';

    bitmatrix_unset(&data, 2, 2);

    ASSERT_EQ(data, '\xff');
}

TEST(bitarray, bitmatrix_unset_when_unflipped_cascade) {
    uint8_t data = '\xff';

    bitmatrix_unset(&data, 2, 0);

    ASSERT_EQ(data, '\xfd');
}

TEST(bitarray, bitmatrix_unset_when_flipped_cascade) {
    uint8_t data = '\xff';

    bitmatrix_unset(&data, 0, 2);

    ASSERT_EQ(data, '\xfd');
}

TEST(bitarray, bitmatrix_get_returns_false_when_null_player) {
    uint8_t data = '\x00';

    ASSERT_FALSE(bitmatrix_get(&data, NULL_PLAYER, 0));
}

TEST(bitarray, bitmatrix_get_returns_true_when_same_players) {
    uint8_t data = '\x00';

    ASSERT_TRUE(bitmatrix_get(&data, 2, 2));
}

TEST(bitarray, bitmatrix_get_when_unflipped_cascade) {
    uint8_t data = '\x02';

    ASSERT_TRUE(bitmatrix_get(&data, 2, 0));
}

TEST(bitarray, bitmatrix_get_when_flipped_cascade) {
    uint8_t data = '\x02';

    ASSERT_TRUE(bitmatrix_get(&data, 0, 2));
}
