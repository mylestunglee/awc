#include "bap_glpk_solver_internal_fixture.hpp"

bap_glpk_solver_internal_fixture::bap_glpk_solver_internal_fixture()
    : managed_inputs(std::make_unique<struct bap_inputs>()),
      managed_temps(std::make_unique<struct bap_temps>()),
      inputs(managed_inputs.get()), temps(managed_temps.get()) {}