#include "units_fixture.hpp"

units_fixture::units_fixture()
    : managed_units(std::make_unique<struct units>()),
      units(managed_units.get()) {
    units_initialise(units);
}

void units_fixture::insert(const struct unit& unit) {
    assert(units_is_insertable(units));
    units_insert(units, &unit);
}