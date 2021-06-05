#include "../list.h"
#include <gtest/gtest.h>

class list_fixture : public ::testing::Test {
  protected:
    list_fixture() : list(new struct list) { list_initialise(list); }
    ~list_fixture() { delete list; }
    struct list* const list;
};

TEST_F(list_fixture, list_initialise_sets_start_and_end) {
    list->start = 2;
    list->end = 3;
    list_initialise(list);
    ASSERT_EQ(list->start, 0);
    ASSERT_EQ(list->end, 0);
}
