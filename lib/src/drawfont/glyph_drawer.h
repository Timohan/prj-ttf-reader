/*!
 * \file
 * \brief file glyph_drawer.h
 *
 * Handles the pixels of glyph and draws the lines
 *
 * Copyright of Timo Hannukkala. All rights reserved.
 *
 * \author Timo Hannukkala <timohannukkala@hotmail.com>
 */

#ifndef GLYPH_DRAWER_H
#define GLYPH_DRAWER_H

#include <stddef.h>
#include <stdint.h>

/*!
 * \brief The pixel_drawing_t struct
 *
 * this is for drawing the glyph image data
 */
typedef struct {
    uint8_t line : 2;
    uint32_t previous_line_path_index;
    uint8_t inner_pixel : 1;
    uint8_t outer_pixel : 1;
    uint8_t completed_x : 1;
    uint8_t completed_y : 1;
    uint8_t line_completed : 1;
} pixel_drawing_t;

/*!
 * \brief The font_drawing_t struct
 *
 * this is for drawing the glyph image data
 */
typedef struct
{
    int width;
    int height;
    int line_min_x, line_max_x;
    int line_min_y, line_max_y;
    pixel_drawing_t *list_pixels;
} font_drawing_t;

int glyph_drawer_init(font_drawing_t *drawing, int width, int height);
void glyph_drawer_clear(font_drawing_t *drawing);

int glyph_drawer_paint_curve(float x0, float y0, float x1, float y1, float curveX, float curveY, font_drawing_t *drawing, uint32_t line_index);
int glyph_drawer_draw_line(int x0, int y0, int x1, int y1, font_drawing_t *drawing, uint32_t line_index);

#endif // GLYPH_DRAWER_H
