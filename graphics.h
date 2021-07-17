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

void render_bar(uint32_t, uint32_t, const grid_t, wchar_t* const,
                uint8_t* const);

bool render_unit_health_bar(const struct units* const, const grid_t,
                            const grid_t, const grid_t, const grid_t,
                            wchar_t* const, uint8_t* const);

bool render_capture_progress_bar(const struct units* const, const grid_t,
                                 const grid_t, const grid_t, const grid_t,
                                 wchar_t* const, uint8_t* const);

uint8_t calc_tile_style(const struct game* const, const grid_t, const grid_t);

uint8_t calc_action_style(const bool, const bool);

uint8_t calc_selection_style(const struct game* const, const grid_t,
                             const grid_t, const bool, const bool);

bool calc_selection_symbol(const grid_t, const grid_t, wchar_t* const);

bool render_selection(const struct game* const, const grid_t, const grid_t,
                      const grid_t, const grid_t, const bool, const bool,
                      wchar_t* const, uint8_t* const);

bool decode_texture(const uint8_t, const bool, const player_t, wchar_t* const,
                    uint8_t* const);

bool render_unit(const struct units* const, const grid_t, const grid_t,
                 const grid_t, const grid_t, wchar_t* const, uint8_t* const);

void render_highlight(const uint8_t label, wchar_t* const symbol,
                      uint8_t* const style);

void render_attack_arrows(const struct game* const, const grid_t,
                          wchar_t* const, uint8_t* const);

bool render_tile(const struct game* const, const grid_t, const grid_t,
                 const grid_t, const grid_t, const bool, wchar_t* const,
                 uint8_t* const);

#endif

#endif
