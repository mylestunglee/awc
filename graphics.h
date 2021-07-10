#ifndef graphics_h
#define graphics_h

#include "game.h"
#include <assert.h>

void render(const struct game* const, const bool, const bool);

void reset_style(void);

void graphics_init(void);

#ifdef expose_graphics_internals

void render_block(uint32_t, uint32_t, const grid_t, wchar_t* const,
                  uint8_t* const);

void render_percentage(uint32_t, uint32_t, const grid_t, wchar_t* const);

void render_progress_bar(uint32_t, uint32_t, const grid_t, wchar_t* const,
                         uint8_t* const);

// ----

bool render_unit(const struct game* const, const grid_t, const grid_t,
                 const grid_t, const grid_t, wchar_t* const, uint8_t* const);

uint8_t calc_tile_style(const struct game* const game, const grid_t,
                        const grid_t);

uint8_t calc_label_style(const bool, const bool);
uint8_t calc_selection_style(const struct game* const, const grid_t,
                             const grid_t, const bool, const bool);
bool calc_selection_symbol(const grid_t, const grid_t, wchar_t* const);
bool render_selection(const struct game* const, const grid_t, const grid_t,
                      const grid_t, const grid_t, const bool, const bool,
                      wchar_t* const, uint8_t* const);

#endif

#endif
