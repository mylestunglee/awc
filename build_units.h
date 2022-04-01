#ifndef BUILD_UNITS_H
#define BUILD_UNITS_H

#include "bap.h"
#include "game.h"

#ifdef EXPOSE_BUILD_UNITS_INTERNAL
void accumulate_distribution(const struct game* const, const player_t,
                             health_wide_t[MODEL_CAPACITY]);
void inputs_initialise_distributions(const struct game* const,
                                     struct bap_inputs* const);
void inputs_initialise_capturables(struct game* const,
                                   struct bap_inputs* const);
void inputs_initialise(struct game* const, struct bap_inputs* const);
void realise_allocations(struct game* const,
                         grid_wide_t allocations[MODEL_CAPACITY]);
#endif
void build_units(struct game* const);

#endif