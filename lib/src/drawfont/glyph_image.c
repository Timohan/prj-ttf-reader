/*!
 * \file
 * \brief file glyph_image.c
 *
 * sets glyph into image x/y position
 *
 * Copyright of Timo Hannukkala. All rights reserved.
 *
 * \author Timo Hannukkala <timo.hannukkala@hotmail.com>
 */

#include "glyph_image.h"
#include <stdlib.h>
#include <errno.h>
#include "glyph_drawer.h"
#include "../prj-ttf-reader.h"

/*!
 * \brief glyph_image_generate_reader_data
 *
 * allocs the data->list_data
 *
 * \param list_sizes_count size of allow
 * \param width image width
 * \param height image height
 * \param data to alloc
 * \return 0 on success
 */
int glyph_image_generate_reader_data(uint32_t list_sizes_count, int32_t width, int32_t height, prj_ttf_reader_data_t *data)
{
    data->image.width = width;
    data->image.height = height;
    data->image.data = (uint8_t *)calloc(1, (uint32_t)(width*height));
    if (!data->image.data) {
        return errno;
    }
    data->list_data_count = list_sizes_count;
    data->list_data = (prj_ttf_reader_glyph_data_t *)malloc( sizeof(prj_ttf_reader_glyph_data_t)*list_sizes_count );
    if (!data->list_data) {
        return errno;
    }
    return 0;
}

/*!
 * \brief glyph_image_add_glyph_into_image
 *
 * add drawing "image" into final image (data)
 *
 * \param list_sizes
 * \param list_index
 * \param drawing
 * \param quality
 * \param data
 * \param offset_y
 * \return 0 on success
 */
int glyph_image_add_glyph_into_image(const font_size_t *list_sizes, int list_index,
                                     const font_drawing_t *drawing, int quality,
                                     prj_ttf_reader_data_t *data, int32_t offset_y)
{
    int x = 0;
    int y = 0;
    int start_x = -1;
    int start_y = -1;
    int end_x = 0;
    int end_y = 0;
    int x_converted;
    int y_converted;
    pixel_drawing_t *p_pixel;
    int quality_multiply = quality*quality;

    int new_width = 1+drawing->width/quality;
    int new_height = 1+drawing->height/quality;

    int *px_count = (int *)calloc((uint32_t)(new_width*new_height), sizeof(int) );
    if (!px_count) {
        return errno;
    }

    for (x=0;x<drawing->width;x++) {
        for (y=0;y<drawing->height;y++) {
            p_pixel = &drawing->list_pixels[y*drawing->width+x];
            if (p_pixel->line || p_pixel->inner_pixel) {
                x_converted = x/quality;
                y_converted = y/quality;
                if (start_x == -1) {
                    start_x = x_converted;
                    start_y = y_converted;
                    end_x = x_converted;
                    end_y = y_converted;
                } else {
                    if (start_x > x_converted) {
                        start_x = x_converted;
                    }
                    if (start_y > y_converted) {
                        start_y = y_converted;
                    }
                    if (end_x < x_converted) {
                        end_x = x_converted;
                    }
                    if (end_y < y_converted) {
                        end_y = y_converted;
                    }
                }

                if (list_sizes[list_index].is_empty) {
                    px_count[ y_converted*new_width+x_converted ]++;
                }
            }
        }
    }

    for (x=start_x;x<=end_x;x++) {
        for (y=start_y;y<=end_y;y++) {
            data->image.data[ ((y-start_y)+list_sizes[list_index].y)*data->image.width+(x-start_x)+list_sizes[list_index].x ] =
                    (uint8_t)(px_count[ (end_y-y+1)*new_width+x ]*255/quality_multiply);
        }
    }

    data->list_data[list_index].image_pixel_left_x = list_sizes[list_index].x;
    data->list_data[list_index].image_pixel_top_y = list_sizes[list_index].y;
    data->list_data[list_index].image_pixel_right_x = ((x-start_x)+list_sizes[list_index].x);
    data->list_data[list_index].image_pixel_bottom_y = ((y-start_y)+list_sizes[list_index].y);
    data->list_data[list_index].image_pixel_offset_line_y = offset_y;

    free(px_count);
    return 0;
}
