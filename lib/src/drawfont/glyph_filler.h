/*!
 * \file
 * \brief file glyph_filler.h
 *
 * Fills the glyph graphics
 *
 * Copyright of Timo Hannukkala. All rights reserved.
 *
 * \author Timo Hannukkala <timo.hannukkala@hotmail.com>
 */

#ifndef GLYPH_FILLER_H
#define GLYPH_FILLER_H

#include <stddef.h>
#include "glyph_drawer.h"

pixel_drawing_t *glyph_filler_get_pixel(font_drawing_t *drawing, int x, int y);
int glyph_filler_draw_inner_area(font_drawing_t *drawing);

#ifdef TIME_DEBUG
void print_glyph_filler_times(void);
#endif // TIME_DEBUG

#endif // GLYPH_FILLER_H
