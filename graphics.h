#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "definitions.h"
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

struct game;

void graphics_initialise(void);
#ifdef EXPOSE_GRAPHICS_INTERNALS
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
void render_highlight(const uint8_t, wchar_t* const, uint8_t* const);
void render_attack_arrows(const struct game* const, const grid_t,
                          wchar_t* const, uint8_t* const);
bool render_tile(const struct game* const, const grid_t, const grid_t,
                 const grid_t, const grid_t, const bool, wchar_t* const,
                 uint8_t* const);
bool render_pixel(const struct game* const, const grid_t, const grid_t,
                  const grid_t, const grid_t, const bool, const bool,
                  wchar_t* const, uint8_t* const);
void apply_style(const uint8_t, const uint8_t);
uint8_t apply_pixel(const struct game* const, const grid_t, const grid_t,
                    const grid_t, const grid_t, const bool, const bool,
                    const uint8_t);
void reset_cursor(void);
void reset_black(void);
void reset_style(void);
void print_normal_text(const struct game* const);
void print_attack_text(const struct game* const);
void print_build_text(const struct game* const);
void print_text(const struct game* const, const bool, const bool);
#endif
void graphics_render(const struct game* const);
#endif
