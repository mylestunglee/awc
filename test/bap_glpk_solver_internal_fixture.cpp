#include "bap_glpk_solver_internal_fixture.hpp"
#include <glpk.h>

bap_glpk_solver_internal_fixture::bap_glpk_solver_internal_fixture()
    : managed_inputs(std::make_unique<struct bap_inputs>()),
      managed_temps(std::make_unique<struct bap_glpk_temps>()),
      inputs(managed_inputs.get()), temps(managed_temps.get()) {
    managed_temps->problem = glp_create_prob();
    managed_temps->curr_index = 1;
    problem = managed_temps->problem;
}