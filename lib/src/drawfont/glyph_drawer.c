/*!
 * \file
 * \brief file glyph_drawer.c
 *
 * Handles the pixels of glyph and draws the lines
 *
 * Copyright of Timo Hannukkala. All rights reserved.
 *
 * \author Timo Hannukkala <timo.hannukkala@hotmail.com>
 */

#include "glyph_drawer.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <errno.h>
#include "glyph_filler.h"

/*!
 * \brief glyph_drawer_init
 *
 * Inits the font_drawing
 *
 * \param drawing
 * \param width
 * \param height
 * \return 0 in success
 */
int glyph_drawer_init(font_drawing_t *drawing, int width, int height)
{
    drawing->width = width;
    drawing->height = height;
    drawing->list_pixels = (pixel_drawing_t *)calloc((uint32_t)(width*height), sizeof(pixel_drawing_t) );
    if (!drawing->list_pixels) {
        return errno;
    }
    return 0;
}

/*!
 * \brief glyph_drawer_clear
 *
 * Clear the font_drawing
 *
 * \param drawing
 */
void glyph_drawer_clear(font_drawing_t *drawing)
{
    free(drawing->list_pixels);
}

/*!
 * \brief glyph_drawer_paint_curve
 *
 * drawing the curve by using bezier curve
 *
 * \param x0 start x postion
 * \param y0 start y postion
 * \param x1 end x postion
 * \param y1 end y postion
 * \param curveX
 * \param curveY
 * \param drawing
 * \param line_path_index value of line (path), to avoid dublicate line value into
 * \return 0
 */
int glyph_drawer_paint_curve(float x0, float y0, float x1, float y1, float curveX, float curveY, font_drawing_t *drawing, uint32_t line_path_index)
{
    if (fabs(x0 - x1) <= 0 && fabs(y0 - y1) <= 0) {
        return 0;
    }
    int i, curve_step;
    float distanceX0 = curveX-x0;
    float distanceY0 = curveY-y0;
    float distanceX1 = curveX-x1;
    float distanceY1 = curveY-y1;

    float positionLineFromBeginning0X;
    float positionLineFromBeginning0Y;
    float positionLineFromEnd0X;
    float positionLineFromEnd0Y;

    float positionLineFromBeginning1X;
    float positionLineFromBeginning1Y;
    float positionLineFromEnd1X;
    float positionLineFromEnd1Y;

    float newX0;
    float newY0;
    float newX1;
    float newY1;

    float prevX0 = x0;
    float prevY0 = y0;
    float prevX1 = x1;
    float prevY1 = y1;

    float difference = fabsf(x0 - x1);
    float differenceTmp = fabsf(y0 - y1);

    if (difference < differenceTmp) {
        difference = differenceTmp;
    }

    curve_step = (int)(50.0f/difference);
    if (curve_step <= 1) {
        curve_step = 1;
    }
    if (curve_step > 25) {
        curve_step = 25;
    }

    for (i=curve_step;i<50;i+=curve_step) {
        positionLineFromBeginning0X =  distanceX0*(float)(i)/100.0f + x0;
        positionLineFromBeginning0Y =  distanceY0*(float)(i)/100.0f + y0;
        positionLineFromBeginning1X =  distanceX1*(float)(i)/100.0f + x1;
        positionLineFromBeginning1Y =  distanceY1*(float)(i)/100.0f + y1;

        positionLineFromEnd0X = distanceX0*(float)(100-i)/100.0f + x0;
        positionLineFromEnd0Y = distanceY0*(float)(100-i)/100.0f + y0;
        positionLineFromEnd1X = distanceX1*(float)(100-i)/100.0f + x1;
        positionLineFromEnd1Y = distanceY1*(float)(100-i)/100.0f + y1;

        newX0 = (positionLineFromEnd1X-positionLineFromBeginning0X)*(float)(i)/100.0f + positionLineFromBeginning0X;
        newY0 = (positionLineFromEnd1Y-positionLineFromBeginning0Y)*(float)(i)/100.0f + positionLineFromBeginning0Y;

        newX1 = (positionLineFromEnd0X-positionLineFromBeginning1X)*(float)(i)/100.0f + positionLineFromBeginning1X;
        newY1 = (positionLineFromEnd0Y-positionLineFromBeginning1Y)*(float)(i)/100.0f + positionLineFromBeginning1Y;

        glyph_drawer_draw_line((int)newX0, (int)newY0, (int)prevX0, (int)prevY0, drawing, line_path_index);
        glyph_drawer_draw_line((int)newX1, (int)newY1, (int)prevX1, (int)prevY1, drawing, line_path_index);

        prevX0 = newX0;
        prevY0 = newY0;
        prevX1 = newX1;
        prevY1 = newY1;
    }
    i = 50;

    positionLineFromBeginning0X =  distanceX0*(float)(i)/100.0f + x0;
    positionLineFromBeginning0Y =  distanceY0*(float)(i)/100.0f + y0;
    positionLineFromBeginning1X =  distanceX1*(float)(i)/100.0f + x1;
    positionLineFromBeginning1Y =  distanceY1*(float)(i)/100.0f + y1;

    newX0 = (positionLineFromBeginning0X + positionLineFromBeginning1X)/2.0f;
    newY0 = (positionLineFromBeginning0Y + positionLineFromBeginning1Y)/2.0f;


   glyph_drawer_draw_line((int)newX0, (int)newY0, (int)prevX0, (int)prevY0, drawing, line_path_index);
   glyph_drawer_draw_line((int)newX0, (int)newY0, (int)prevX1, (int)prevY1, drawing, line_path_index);

   return 0;
}

/*!
 * \brief glyph_drawer_add_line_value
 *
 * Add/set line pixel into x/y
 *
 * \param x
 * \param y
 * \param drawing
 * \param line_path_index to avoid line++ if the pixel is got line from previous line
 * -> this can happen in cross"roads" and curves
 */
#ifndef TEST_CASE
static
#endif
void glyph_drawer_add_line_value(int x, int y, font_drawing_t *drawing, uint32_t line_path_index)
{
    int pixel_index = y*drawing->width+x;
    // if line count == 2 -> no need to continue
    if (drawing->list_pixels[pixel_index].line == 2) {
        return;
    }

    if (drawing->list_pixels[pixel_index].line == 1) {
        if (line_path_index != drawing->list_pixels[pixel_index].previous_line_path_index) {
            drawing->list_pixels[pixel_index].line++;
        }
        return;
    }

    // line pixel is new
    if (!drawing->line_max_x && !drawing->line_max_y) {
        // first line pixel ever
        drawing->line_min_x = x;
        drawing->line_max_x = x;
        drawing->line_min_y = y;
        drawing->line_max_y = y;
    } else {
        // not first pixel line ever, so check min/max x/y
        if (drawing->line_min_x > x) {
            drawing->line_min_x = x;
        }
        if (drawing->line_min_y > y) {
            drawing->line_min_y = y;
        }
        if (drawing->line_max_x < x) {
            drawing->line_max_x = x;
        }
        if (drawing->line_max_y < y) {
            drawing->line_max_y = y;
        }
    }

    drawing->list_pixels[pixel_index].line = 1;
    drawing->list_pixels[pixel_index].previous_line_path_index = line_path_index;
}

/*!
 * \brief glyph_drawer_draw_line
 *
 * Drawing the line between x0/y0 to x1/y1
 *
 * \param x0
 * \param y0
 * \param x1
 * \param y1
 * \param drawing
 * \param line_path_index value of line (path), to avoid dublicate line value into
 * \return 0 on success
 */
int glyph_drawer_draw_line(int x0, int y0, int x1, int y1, font_drawing_t *drawing, uint32_t line_path_index)
{
    if (x0 >= drawing->width || x1 >= drawing->width || y0 >= drawing->height || y1 >= drawing->height) {
        return 0;
    }

    int x;
    int y;

    if (x0 == x1) {
        if (y0 == y1) {
            glyph_drawer_add_line_value(x0, y0, drawing, line_path_index);
        } else if (y0 < y1) {
            for (y=y0;y<=y1;y++) {
                glyph_drawer_add_line_value(x0, y, drawing, line_path_index);
            }
        } else {
            for (y=y1;y<=y0;y++) {
                glyph_drawer_add_line_value(x0, y, drawing, line_path_index);
            }
        }
        return 0;
    }

    if (y0 == y1) {
        if (x0 < x1) {
            for (x=x0;x<=x1;x++) {
                glyph_drawer_add_line_value(x, y0, drawing, line_path_index);
            }
        } else {
            for (x=x1;x<=x0;x++) {
                glyph_drawer_add_line_value(x, y0, drawing, line_path_index);
            }
        }
        return 0;
    }

    int wY;
    int wX;
    if (x0 < x1) {
        if (y0 < y1) {
            wX = x1 - x0;
            wY = y1 - y0;
            if (wY >= wX) {
                for (y=y0;y<=y1;y++) {
                    x = (y-y0)*wX/wY + x0;
                    glyph_drawer_add_line_value(x, y, drawing, line_path_index);
                }
            } else {
                for (x=x0;x<=x1;x++) {
                    y = (x-x0)*wY/wX + y0;
                    glyph_drawer_add_line_value(x, y, drawing, line_path_index);
                }
            }
        } else {
            wX = x1 - x0;
            wY = y0 - y1;
            if (wY >= wX) {
                for (y=y1;y<=y0;y++) {
                    x = x1 - (y-y1)*wX/wY;
                    glyph_drawer_add_line_value(x, y, drawing, line_path_index);
                }
            } else {
                for (x=x0;x<=x1;x++) {
                    y = y0 - (x-x0)*wY/wX;
                    glyph_drawer_add_line_value(x, y, drawing, line_path_index);
                }
            }
        }
        return 0;
    }

    if (x0 > x1) {
        if (y0 < y1) {
            wX = x0 - x1;
            wY = y1 - y0;
            if (wY >= wX) {
                for (y=y0;y<=y1;y++) {
                    x = x0 - (y-y0)*wX/wY;
                    glyph_drawer_add_line_value(x, y, drawing, line_path_index);
                }
            } else {
                for (x=x1;x<=x0;x++) {
                    y = y1 - (x-x1)*wY/wX;
                    glyph_drawer_add_line_value(x, y, drawing, line_path_index);
                }
            }
        }
        else {
            wX = x0 - x1;
            wY = y0 - y1;
            if (wY >= wX) {
                for (y=y1;y<=y0;y++) {
                    x = x0 - (y-y1)*wX/wY;
                    glyph_drawer_add_line_value(x, y0-(y-y1), drawing, line_path_index);
                }
            } else {
                for (x=x1;x<=x0;x++) {
                    y = y0 - (x-x1)*wY/wX;
                    glyph_drawer_add_line_value(x, y1-(y-y0), drawing, line_path_index);
                }
            }
        }
        return 0;
    }

    return EPERM;
}
