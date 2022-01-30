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

TEST_F(list_fixture, list_initialise_list_is_empty) {
    ASSERT_TRUE(list_empty(list));
}

TEST_F(list_fixture, list_insert_increments_end) {
    list->end = 2;
    struct list_node node;
    list_insert(list, &node);
    ASSERT_EQ(list->end, 3);
}

TEST_F(list_fixture, list_insert_inserts_node) {
    list->end = 2;
    struct list_node node = {.x = 3, .y = 5, .energy = 7};
    list_insert(list, &node);
    ASSERT_EQ(list->nodes[2].x, 3);
    ASSERT_EQ(list->nodes[2].y, 5);
    ASSERT_EQ(list->nodes[2].energy, 7);
}

TEST_F(list_fixture, list_insert_does_not_insert_when_full) {
    list->start = 3;
    list->end = 2;
    struct list_node node;
    list_insert(list, &node);
    ASSERT_EQ(list->start, 3);
    ASSERT_EQ(list->end, 2);
}

TEST_F(list_fixture, list_front_pop_increments_start) {
    list->start = 2;
    list->end = 5;
    list_front_pop(list);
    ASSERT_EQ(list->start, 3);
}

TEST_F(list_fixture, list_front_pop_returns_first_node) {
    list->start = 2;
    list->end = 5;
    list->nodes[2] = {.x = 3, .y = 5, .energy = 7};
    auto node = list_front_pop(list);
    ASSERT_EQ(node.x, list->nodes[2].x);
    ASSERT_EQ(node.y, list->nodes[2].y);
    ASSERT_EQ(node.energy, list->nodes[2].energy);
}

TEST_F(list_fixture, list_back_pop_decrements_end) {
    list->start = 2;
    list->end = 5;
    list_back_pop(list);
    ASSERT_EQ(list->end, 4);
}

TEST_F(list_fixture, list_back_pop_returns_last_node) {
    list->start = 2;
    list->end = 5;
    list->nodes[4] = {.x = 3, .y = 5, .energy = 7};
    auto node = list_back_pop(list);
    ASSERT_EQ(node.x, list->nodes[4].x);
    ASSERT_EQ(node.y, list->nodes[4].y);
    ASSERT_EQ(node.energy, list->nodes[4].energy);
}

TEST_F(list_fixture, list_front_peek_does_not_change_start_or_end) {
    list->start = 2;
    list->end = 5;
    list_front_peek(list);
    ASSERT_EQ(list->start, 2);
    ASSERT_EQ(list->end, 5);
}

TEST_F(list_fixture, list_front_peek_returns_first_node) {
    list->start = 2;
    list->end = 5;
    list->nodes[2] = {.x = 3, .y = 5, .energy = 7};
    auto node = list_front_peek(list);
    ASSERT_EQ(node.x, list->nodes[2].x);
    ASSERT_EQ(node.y, list->nodes[2].y);
    ASSERT_EQ(node.energy, list->nodes[2].energy);
}

TEST_F(list_fixture, list_back_peek_returns_last_node) {
    list->start = 2;
    list->end = 5;
    list->nodes[4] = {.x = 3, .y = 5, .energy = 7};
    auto node = list_back_peek(list);
    ASSERT_EQ(node.x, list->nodes[4].x);
    ASSERT_EQ(node.y, list->nodes[4].y);
    ASSERT_EQ(node.energy, list->nodes[4].energy);
}
