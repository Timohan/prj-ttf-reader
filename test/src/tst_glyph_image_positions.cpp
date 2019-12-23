/*!
* \file
* \brief file tst_glyph_image_positions.cpp
*
* glyph_image_positions unit tests
*
* Copyright of Timo Hannukkala, All rights reserved.
*
* \author Timo Hannukkala <timohannukkala@hotmail.com>
*/
#include "tst_glyph_image_positions.h"
#include <stdlib.h>
#include <stdio.h>
#include "../../lib/src/drawfont/glyph_image_positions.h"
#include "../../lib/src/drawfont/glyph_image.h"

/*!
 * \brief tst_glyph_image_positions_generate_glyph_positions
 *
 * tests glyph_image_positions_generate_glyph_positions
 *
 * \return 0 on success
 */
int tst_glyph_image_positions_generate_glyph_positions()
{
    int i, i2;
    int width, height;
    font_size_t *list_size;
    int list_size_count;

    list_size = (font_size_t *)malloc(sizeof(font_size_t) );
    list_size_count = 1;


    for (i=0;i<2;i++) {
        list_size[0].x = -1;
        list_size[0].y = -1;
        list_size[0].width = 3+i;
        list_size[0].height = 3+i;
        width = 0;
        height = 0;

        glyph_image_positions_generate_glyph_positions(list_size, list_size_count, &width, &height);

        if (width != 4 || height != 4 || list_size[0].x  || list_size[0].y) {
            return 1;
        }
    }

    for (i=0;i<2;i++) {
        list_size[0].x = -1;
        list_size[0].y = -1;
        list_size[0].width = 5+i;
        list_size[0].height = 5+i;
        width = 0;
        height = 0;

        glyph_image_positions_generate_glyph_positions(list_size, list_size_count, &width, &height);

        if (width != 8 || height != 8 || list_size[0].x  || list_size[0].y) {
            return 2;
        }
    }

    free(list_size);

    list_size = (font_size_t *)malloc(sizeof(font_size_t)*2 );
    list_size_count = 2;

    for (i=0;i<list_size_count;i++) {
        list_size[i].x = -1;
        list_size[i].y = -1;
        list_size[i].width = 6;
        list_size[i].height = 3;
    }
    width = 0;
    height = 0;
    glyph_image_positions_generate_glyph_positions(list_size, list_size_count, &width, &height);

    if (width != 8 || height != 8 || list_size[0].x  || list_size[0].y  || list_size[1].x != 0 || list_size[1].y != 3) {
        return 3;
    }

    for (i=0;i<list_size_count;i++) {
        list_size[i].x = -1;
        list_size[i].y = -1;
        list_size[i].width = 3;
        list_size[i].height = 6;
    }
    width = 0;
    height = 0;
    glyph_image_positions_generate_glyph_positions(list_size, list_size_count, &width, &height);

    if (width != 8 || height != 8 || list_size[0].x  || list_size[0].y  || list_size[1].x != 3 || list_size[1].y != 0) {
        return 3;
    }

    free(list_size);

    list_size = (font_size_t *)malloc(sizeof(font_size_t)*4 );
    list_size_count = 4;

    for (i=0;i<list_size_count;i++) {
        list_size[i].x = -1;
        list_size[i].y = -1;
        list_size[i].width = 6;
        list_size[i].height = 3;
    }
    width = 0;
    height = 0;
    glyph_image_positions_generate_glyph_positions(list_size, list_size_count, &width, &height);
    if (width != 16 || height != 16) {
        return 4;
    }

    if (list_size[0].x != 0 || list_size[0].y != 0
            || list_size[1].x != 0 || list_size[1].y != 3
            || list_size[2].x != 0 || list_size[2].y != 6
            || list_size[3].x != 6 || list_size[3].y != 0) {
        return 5;
    }

    free(list_size);

    list_size = (font_size_t *)malloc(sizeof(font_size_t)*4 );
    list_size_count = 4;

    for (i=0;i<list_size_count;i++) {
        list_size[i].x = -1;
        list_size[i].y = -1;
        list_size[i].width = 6;
        list_size[i].height = 3;
    }
    width = 0;
    height = 0;
    glyph_image_positions_generate_glyph_positions(list_size, list_size_count, &width, &height);
    if (width != 16 || height != 16) {
        return 4;
    }

    if (list_size[0].x != 0 || list_size[0].y != 0
            || list_size[1].x != 0 || list_size[1].y != 3
            || list_size[2].x != 0 || list_size[2].y != 6
            || list_size[3].x != 6 || list_size[3].y != 0) {
        return 5;
    }

    for (i=0;i<list_size_count;i++) {
        list_size[i].x = -1;
        list_size[i].y = -1;
        list_size[i].width = 3;
        list_size[i].height = 6;
    }
    width = 0;
    height = 0;
    glyph_image_positions_generate_glyph_positions(list_size, list_size_count, &width, &height);
    if (width != 16 || height != 8) {
        return 6;
    }

    if (list_size[0].x != 0 || list_size[0].y != 0
            || list_size[1].x != 3 || list_size[1].y != 0
            || list_size[2].x != 6 || list_size[2].y != 0
            || list_size[3].x != 9 || list_size[3].y != 0) {
        return 7;
    }

    free(list_size);

    list_size = (font_size_t *)malloc(sizeof(font_size_t)*20 );
    list_size_count = 20;

    for (i=0;i<list_size_count;i++) {
        list_size[i].x = -1;
        list_size[i].y = -1;
        list_size[i].width = 6;
        list_size[i].height = 3;
    }
    width = 0;
    height = 0;
    glyph_image_positions_generate_glyph_positions(list_size, list_size_count, &width, &height);

    if (width != 32 || height != 32) {
        return 8;
    }

    for (i=0;i<list_size_count;i++) {
        for (i2=0;i2<list_size_count;i2++) {
            if (i != i2) {
                if (list_size[i].x == list_size[i2].x && list_size[i].y == list_size[i2].y) {
                    return 9;
                }
            }
        }
    }

    free(list_size);

    return 0;
}
