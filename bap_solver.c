#include "bap_solver.h"
#include "bap_default_solver.h"
#include "bap_glpk_solver.h"

void bap_solve(const struct bap_inputs* const inputs,
               grid_wide_t outputs[MODEL_CAPACITY], void* const workspace) {
    if (bap_glpk_solvable(inputs))
        bap_glpk_solve(inputs, outputs, workspace);
    else
        bap_default_solve(inputs, outputs);
}
