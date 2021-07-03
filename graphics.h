#ifndef graphics_h
#define graphics_h

#include "game.h"
#include <assert.h>

void render(const struct game* const, const bool, const bool);

void reset_style(void);

void graphics_init(void);

#ifdef expose_graphics_internals

bool render_unit(const struct game* const, const grid_t, const grid_t,
                 const grid_t, const grid_t, wchar_t* const, uint8_t* const);

#endif

#endif
