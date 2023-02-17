#include "../src/console.h"
#include <gtest/gtest.h>

TEST(console_coverage_test, getch) { ASSERT_EQ(getch(), 0); }

TEST(console_coverage_test, get_console_size) {
    int dummy;

    get_console_size(&dummy, &dummy);
}
