#define EXPOSE_BAP_GLPK_SOLVER_INTERNALS
#include "bap_glpk_solver_internal_fixture.hpp"
#include <glpk.h>

bap_glpk_solver_internal_fixture::bap_glpk_solver_internal_fixture()
    : managed_inputs(std::make_unique<struct bap_inputs>()),
      managed_temps(std::make_unique<struct bap_glpk_temps>()),
      inputs(managed_inputs.get()), temps(managed_temps.get()),
      problem(managed_temps->problem) {
    bap_glpk_temps_preinitialise(temps);
    temps->curr_index = 1;
}
