#ifndef UNITS_FIXTURE_H
#define UNITS_FIXTURE_H

#include "../units.h"
#include <gtest/gtest.h>
#include <memory>

class units_fixture : public ::testing::Test {
protected:
    units_fixture();
    void insert(const struct unit& unit);

private:
    std::unique_ptr<struct units> managed_units;

protected:
    struct units* const units;
};

#endif
