#include "bap_solver_fixture.hpp"
#include "../bap_solver.h"

bap_solver_fixture::bap_solver_fixture()
    : inputs{0}, outputs{0}, workspace(std::make_unique<struct list>()) {}

void bap_solver_fixture::solve() {
    bap_solve(&inputs, outputs, workspace.get());
}