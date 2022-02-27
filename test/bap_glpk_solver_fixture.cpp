#include "bap_glpk_solver_fixture.hpp"

bap_glpk_solver_fixture::bap_glpk_solver_fixture()
    : inputs(std::make_unique<struct bap_inputs>()), outputs{0},
      workspace(std::make_unique<struct list>()) {}

int bap_glpk_solver_fixture::solve() {
    return bap_glpk_solve(inputs.get(), outputs, workspace.get());
}