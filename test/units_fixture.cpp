#include "units_fixture.hpp"

units_fixture::units_fixture() : units(new struct units) {
    units_initialise(units);
}

units_fixture::~units_fixture() { delete units; }

void units_fixture::insert(const struct unit& unit) {
    units_insert(units, &unit);
}