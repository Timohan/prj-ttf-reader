/*!
 * \file
 * \brief file glyph_image_positions.c
 *
 * Set glyph's x/y positions into upcoming image of all
 * glyphs
 *
 * Copyright of Timo Hannukkala. All rights reserved.
 *
 * \author Timo Hannukkala <timohannukkala@hotmail.com>
 */

#include "glyph_image_positions.h"
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

/*!
 * \brief generate_glyph_positions_add
 *
 * Add glyph font size positions (x/y)
 *
 * \param size fills the x/y
 * \param x new x
 * \param y new y
 * \param list_most_right_position [in/out] list of most right positions, it will be resized if required
 * \param list_most_right_position_size [in/out] size of most right positions
 * \param max_x [in/out] new x if the new size will require it
 * \param max_y [in/out] new y if the new size will require it
 * \return 0 on success
 */
static int generate_glyph_positions_add(font_size_t *size, int x, int y,
                                        int **list_most_right_position,
                                        int *list_most_right_position_size,
                                        int *max_x,
                                        int *max_y)
{
    int i;
    int *tmp;
    int current_size = *list_most_right_position_size;
    size->x = x;
    size->y = y;

    if (size->y+size->height >= current_size) {
        *list_most_right_position_size += size->height*2;
        tmp = (int *)realloc(*list_most_right_position, (uint32_t)(*list_most_right_position_size)*sizeof(int) );
        if (!tmp) {
            return errno;
        }
        *list_most_right_position = tmp;
        memset((*list_most_right_position)+current_size, 0, (uint32_t)(*list_most_right_position_size-current_size)*sizeof(int) );
    }

    for (i=size->y;i<size->y+size->height;i++) {
        (*list_most_right_position)[i] = size->x+size->width;
    }

    if (*max_y < size->y+size->height) {
        *max_y = size->y+size->height;
    }

    if (*max_x < size->x+size->width) {
        *max_x = size->x+size->width;
    }
    return 0;
}

/*!
 * \brief get_required_max_value
 *
 * get required max value (2^x) from "from_value"
 *
 * \param from_value
 * \return
 */
static int get_required_max_value(int from_value) {
    int i = 2;
    for (;;) {
        if (i >= from_value) {
            return i;
        }
        i <<= 1;
    }
}

/*!
 * \brief get_most_right_x_from_list
 *
 * gets the most right value for y (and using the height)
 *
 * \param list_most_right_position
 * \param y
 * \param height
 * \return most right
 */
static int get_most_right_x_from_list(const int *list_most_right_position, int y, int height)
{
    int i;
    int most_right = list_most_right_position[y];
    for (i=y+1;i<y+height;++i) {
        if (most_right < list_most_right_position[i]) {
            most_right = list_most_right_position[i];
        }
    }
    return most_right;
}

/*!
 * \brief glyph_image_positions_generate_glyph_positions
 *
 * Generate positions for glyphs
 *
 * in list_sizes width and height are set before, but this function
 * sets x and y positions
 *
 * \param list_sizes [in/out] list of font sizes
 * \param list_sizes_count count of list_sizes
 * \param required_width [out] required width
 * \param required_height [out] required height
 * \return 0 on success
 */
int glyph_image_positions_generate_glyph_positions(font_size_t *list_sizes, int list_sizes_count, int *required_width, int *required_height)
{
    if (list_sizes_count == 0) {
        *required_width = 0;
        *required_height = 0;
        return 0;
    }

    int ret;
    int i;
    int x, y;
    int new_x, new_y;
    int max_y = 0;
    int max_x = 0;
    int *list_most_right_position = (int *)calloc(1024, sizeof(int));
    int list_most_right_position_size = 1024;

    ret = generate_glyph_positions_add(&list_sizes[0], 0, 0, &list_most_right_position, &list_most_right_position_size, &max_x, &max_y);
    if (ret) {
        return ret;
    }

    for (i=1;i<list_sizes_count;++i) {
        if (max_x + list_sizes[i].width > max_y + list_sizes[i].height) {
            // we add this font glyph into new line
            ret = generate_glyph_positions_add(&list_sizes[i], 0, max_y, &list_most_right_position, &list_most_right_position_size, &max_x, &max_y);
            if (ret) {
                return ret;
            }
        } else {
            // we add this font glyph into existing "lines", but find most suitable slot for this
            new_y = 0;
            new_x = get_most_right_x_from_list(list_most_right_position, 0, list_sizes[i].height);
            for (y=1;y<=max_y - list_sizes[i].height;y++) {
                x = get_most_right_x_from_list(list_most_right_position, y, list_sizes[i].height);
                if (x < new_x) {
                    new_x = x;
                    new_y = y;
                }
            }
            ret = generate_glyph_positions_add(&list_sizes[i], new_x, new_y, &list_most_right_position, &list_most_right_position_size, &max_x, &max_y);
            if (ret) {
                return ret;
            }
        }
    }

    free(list_most_right_position);

    *required_width = get_required_max_value(max_x);
    *required_height = get_required_max_value(max_y);
    return 0;
}
