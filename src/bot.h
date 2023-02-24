#ifndef BOT_H
#define BOT_H
#include "game.h"

#ifdef EXPOSE_BOT_INTERNALS
void simulate_attack(struct game* const, health_t* const, health_t* const);
const struct unit* find_attackee(struct game* const, const model_t);
void handle_attack(struct game* const, const model_t);
energy_t update_max_energy(const struct game* const, const energy_t,
                           grid_t* const, grid_t* const);
energy_t find_nearest_building(struct game* const);
void handle_capture(struct game* const, const model_t);
void handle_local(struct game* const, const struct unit* const);
energy_t find_nearest_attackable_attackee_ranged(struct game* const,
                                                 const model_t,
                                                 const struct unit* const,
                                                 energy_t, grid_t* const,
                                                 grid_t* const);
energy_t find_nearest_attackable_attackee_direct(struct game* const,
                                                 const struct unit* const,
                                                 energy_t, grid_t* const,
                                                 grid_t* const);
energy_t find_nearest_attackable_attackee(struct game* const, const model_t,
                                          const struct unit* const, energy_t,
                                          grid_t* const, grid_t* const);
energy_t find_nearest_attackable(struct game* const, const model_t,
                                 grid_t* const, grid_t* const);
bool find_nearest_target(struct game* const, const model_t, grid_t* const,
                         grid_t* const);
void move_towards_target(struct game* const, const model_t, const grid_t,
                         const grid_t);
void handle_nonlocal(struct game* const, struct unit* const);
void interact_unit(struct game* const, struct unit* const);
void interact_units(struct game* const);
#endif
void bot_play(struct game* const game);

#endif
