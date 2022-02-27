#ifndef UNITS_FIXTURE_H
#define UNITS_FIXTURE_H

#include "../units.h"
#include <gtest/gtest.h>

class units_fixture : public ::testing::Test {
protected:
    units_fixture();
    ~units_fixture();
    void insert(const struct unit& unit);
    struct units* const units;
};

#endif