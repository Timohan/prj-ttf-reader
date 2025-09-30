/*!
 * \file
 * \brief file glyph_graph_generator.c
 *
 * Generates the glyph into image
 *
 * Copyright of Timo Hannukkala. All rights reserved.
 *
 * \author Timo Hannukkala <timohannukkala@hotmail.com>
 */

#include "glyph_graph_generator.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "../font_tables.h"
#include "../prj-ttf-reader.h"
#include "glyph_image.h"
#include "glyph_image_positions.h"
#include "glyph_drawer.h"
#include "glyph_filler.h"
#include "rotate_math.h"

/*!
 * \brief glyph_graph_generator_is_valid_character
 *
 * is character valid for generating the glyph
 *
 * \param list_characters
 * \param list_characters_size
 * \param character
 * \return 0 if it's valid character
 */
static int glyph_graph_generator_is_valid_character(const uint32_t *list_characters, uint32_t list_characters_size, uint32_t character)
{
    uint32_t i;
    for (i=0;i<list_characters_size;i++) {
        if (character == list_characters[i]) {
            return 0;
        }
    }

    return -1;
}

/*!
 * \brief decrease_min_value
 *
 * decrease value to be smaller than value_to_correct
 *
 * \param value calcualted value that min value could be
 * \param quality
 * \param value_to_correct makes sure that min value must be smaller than this value
 * \return
 */
#ifndef TEST_CASE
static
#endif
int32_t decrease_min_value(int32_t value, int32_t quality, const float value_to_correct)
{
    if (value < (int32_t)(value_to_correct) -1) {
        return value;
    }
    int32_t diff = (value - (int32_t)(value_to_correct))/quality;
    return value - quality*(diff+1);
}

/*!
 * \brief increase_max_value
 *
 * increase value to be bigger than value_to_correct
 *
 * \param value calcualted value that max value could be
 * \param quality
 * \param value_to_correct makes sure that max value must be bigger than this value
 * \return
 */
#ifndef TEST_CASE
static
#endif
int32_t increase_max_value(int32_t value, int32_t quality, const float value_to_correct)
{
    if (value > (int32_t)(value_to_correct)) {
        return value;
    }
    int32_t diff = ((int32_t)(value_to_correct) - value)/quality;
    return value + quality*(diff+1);
}

/*!
 * \brief get_min_value
 *
 * get min x/y value for drawing area
 * -> the glyph will be drawed to this
 * area first time
 *
 * \param value original glyph's max x/y value*quality
 * \param quality
 * \return
 */
#ifndef TEST_CASE
static
#endif
int32_t get_min_value(float value, int quality)
{
    int i_value = (int)value;

    if (fabs((float)(i_value) - value) <= 0 && i_value % quality == 0) {
        return i_value-quality;
    }

    i_value -= i_value % quality;
    if (value < 0) {
        return i_value - quality*2;
    }
    return i_value - quality;
}

static void set_min_max(float *min, float *max, const float new_value)
{
    if (new_value < *min) {
        *min = new_value;
    }
    if (new_value > *max) {
        *max = new_value;
    }
}

/*!
 * \brief get_max_value
 *
 * get max x/y value for drawing area
 * -> the glyph will be drawed to this
 * area first time
 *
 * \param value original glyph's max x/y value*quality
 * \param quality
 * \return
 */
#ifndef TEST_CASE
static
#endif
int32_t get_max_value(float value, int quality)
{
    int i_value = (int)value;

    if (i_value % quality == 0 && fabs((float)i_value - value) <= 0) {
        return i_value+quality;
    }

    i_value -= i_value % quality;
    if (value > 0) {
        return i_value + quality*2;
    }
    return i_value + quality;
}

#ifndef TEST_CASE
/*!
 * \brief glyph_graph_generator_generate_graph
 *
 * generates the glyphs into graphic image
 *
 * \param list_characters
 * \param list_characters_size
 * \param data
 * \param data_size
 * \param font_size_px
 * \param tables
 * \param quality
 * \param image_data
 * \param hor_metrics_table
 * \param hor_header_table
 * \param rotate
 * \param move_glyph_x
 * \param move_glyph_y
 * \return 0 == success
 */
int glyph_graph_generator_generate_graph(const uint32_t *list_characters, uint32_t list_characters_size,
                                         const uint8_t *data, size_t data_size, float font_size_px,
                                         font_tables_t *tables, int quality,
                                         prj_ttf_reader_data_t *image_data,
                                         const horizontal_metrics_table_t *hor_metrics_table,
                                         const horizontal_header_table_t *hor_header_table,
                                         const float rotate,
                                         const float move_glyph_x, const float move_glyph_y)
{
    const float rate = (float)quality*font_size_px/(float)tables->header_table.units_per_em;
    uint16_t index_glyf;
    uint16_t i;
    int list_index = 0;
    uint32_t i2, i3;
    uint32_t line__draw_index = 0;
    float min_x, min_y;
    float max_x, max_y;
    int required_width, required_height;
    float rotated_min_x = 0;
    float rotated_min_y = 0;
    float rotated_max_x = 0;
    float rotated_max_y = 0;
    int first_time;
    int is_empty;
    int ret;
    int32_t tmpi;
    font_size_t *tmp;
    font_drawing_t font_draw;
    float rotated_x[3];
    float rotated_y[3];
    float calculated_x, calculated_y;

    memset(&font_draw, 0, sizeof(font_draw));

    index_glyf = otff_get_table_record_index(tables->list_table_record, tables->offsets.num_tables, "glyf");
    if (UINT16_MAX == index_glyf) {
        return EIO;
    }

    if (glyf_alloc(&tables->list_glyph, &tables->max_profile)) {
        return EIO;
    }

    // first find every characters required size for the image
    for (i=0;i<tables->max_profile.glyphs_count;i++) {
        if (glyph_graph_generator_is_valid_character(list_characters,
                                                     list_characters_size,
                                                     tables->corr_character_table.character[i])) {
             continue;
        }

        is_empty = 1;
        if (i < tables->max_profile.glyphs_count - 1
                && tables->list_index_loc_to_tables[i].offset == tables->list_index_loc_to_tables[i+1].offset) {
            is_empty = 0;
        }
        ret = glyf_parse(data, data_size, i,
                        tables->list_glyph,
                        tables->list_table_record[index_glyf].offset, tables->list_index_loc_to_tables, &tables->max_profile);
        if (ret) {
            return ret;
        }

        min_x = tables->list_glyph[i].min_x;
        min_y = tables->list_glyph[i].min_y;
        max_x = tables->list_glyph[i].max_x;
        max_y = tables->list_glyph[i].max_y;

        if (fabs(min_x) <= 0 && fabs(min_y) <= 0 && fabs(max_x) <= 0 && fabs(max_y) <= 0) {
            continue;
        }

        rotated_min_x = 0;
        rotated_min_y = 0;
        rotated_max_x = 0;
        rotated_max_y = 0;
        first_time = 1;

        for (i2=0;i2<tables->list_glyph[i].list_path_size;i2++) {
            for (i3=0;i3<tables->list_glyph[i].list_path[i2].list_glyph_curve_size;i3++) {
                if (first_time) {
                    first_time = 0;
                    rotate_by_angle_zero(&rotated_min_x, &rotated_min_y,
                                    tables->list_glyph[i].list_path[i2].list_glyph_curve[i3].x0,
                                    tables->list_glyph[i].list_path[i2].list_glyph_curve[i3].y0,
                                    rotate);
                    rotated_max_x = rotated_min_x;
                    rotated_max_y = rotated_min_y;
                } else {
                    rotate_by_angle_zero(&calculated_x, &calculated_y,
                                    tables->list_glyph[i].list_path[i2].list_glyph_curve[i3].x0,
                                    tables->list_glyph[i].list_path[i2].list_glyph_curve[i3].y0,
                                    rotate);
                    set_min_max(&rotated_min_x, &rotated_max_x, calculated_x);
                    set_min_max(&rotated_min_y, &rotated_max_y, calculated_y);
                }
                rotate_by_angle_zero(&calculated_x, &calculated_y,
                                tables->list_glyph[i].list_path[i2].list_glyph_curve[i3].x1,
                                tables->list_glyph[i].list_path[i2].list_glyph_curve[i3].y1,
                                rotate);
                set_min_max(&rotated_min_x, &rotated_max_x, calculated_x);
                set_min_max(&rotated_min_y, &rotated_max_y, calculated_y);

                if (!tables->list_glyph[i].list_path[i2].list_glyph_curve[i3].is_curve) {
                    // it's line, curve points are ignored
                    continue;
                }

                rotate_by_angle_zero(&calculated_x, &calculated_y,
                                tables->list_glyph[i].list_path[i2].list_glyph_curve[i3].curve_x,
                                tables->list_glyph[i].list_path[i2].list_glyph_curve[i3].curve_y,
                                rotate);
                set_min_max(&rotated_min_x, &rotated_max_x, calculated_x);
                set_min_max(&rotated_min_y, &rotated_max_y, calculated_y);
            }
        }

        if (tables->list_font_sizes_count == 0) {
            tables->list_font_sizes = (font_size_t *)malloc(sizeof(font_size_t));
            if (!tables->list_font_sizes) {
                return errno;
            }
        } else {
            tmp = (font_size_t *)realloc(tables->list_font_sizes, sizeof(font_size_t)*(uint32_t)(tables->list_font_sizes_count+1));
            if (!tmp) {
                return errno;
            }
            tables->list_font_sizes = tmp;
        }

        tables->list_font_sizes[tables->list_font_sizes_count].x = -1;
        tables->list_font_sizes[tables->list_font_sizes_count].y = -1;
        tables->list_font_sizes[tables->list_font_sizes_count].width = (int)( (float)(rotated_max_x-rotated_min_x)*rate/(float)quality) + 3;
        tables->list_font_sizes[tables->list_font_sizes_count].height = (int)( (float)(rotated_max_y-rotated_min_y)*rate/(float)quality) + 3;
        tables->list_font_sizes[tables->list_font_sizes_count].rotated_min_x = rotated_min_x;
        tables->list_font_sizes[tables->list_font_sizes_count].rotated_max_x = rotated_max_x;
        tables->list_font_sizes[tables->list_font_sizes_count].rotated_min_y = rotated_min_y;
        tables->list_font_sizes[tables->list_font_sizes_count].rotated_max_y = rotated_max_y;
        tables->list_font_sizes[tables->list_font_sizes_count].is_empty = is_empty;
        tables->list_font_sizes_count++;
    }

    // set positions for image, that contains the all glyphs
    // also we get the required width/height for thei image
    ret = glyph_image_positions_generate_glyph_positions(tables->list_font_sizes, tables->list_font_sizes_count, &required_width, &required_height);
    if (ret) {
        return ret;
    }
    // alloc image data
    ret = glyph_image_generate_reader_data((uint32_t)tables->list_font_sizes_count,
                                           required_width, required_height,
                                           image_data);
    if (ret) {
        return ret;
    }

    // draw lines
    for (i=0;i<tables->max_profile.glyphs_count;i++) {
        if (glyph_graph_generator_is_valid_character(list_characters,
                                                     list_characters_size,
                                                     tables->corr_character_table.character[i])) {
             continue;
        }

        if (tables->list_glyph[i].min_x == 0 && tables->list_glyph[i].min_y == 0
                && tables->list_glyph[i].max_x == 0 && tables->list_glyph[i].max_y == 0) {
            continue;
        }

        min_x = tables->list_glyph[i].min_x;
        min_y = tables->list_glyph[i].min_y;
        max_x = tables->list_glyph[i].max_x;
        max_y = tables->list_glyph[i].max_y;

        tmpi = get_max_value(max_x*rate, quality);
        tmpi = increase_max_value(tmpi, quality, tables->list_font_sizes[list_index].rotated_max_x*rate);
        max_x = (float)(tmpi+quality);
        tmpi = get_max_value(max_y*rate, quality);
        tmpi = increase_max_value(tmpi, quality, tables->list_font_sizes[list_index].rotated_max_y*rate);
        max_y = (float)(tmpi+quality);
        tmpi = get_min_value(min_x*rate, quality);
        tmpi = decrease_min_value(tmpi, quality, tables->list_font_sizes[list_index].rotated_min_x*rate);
        min_x = (float)(tmpi-quality);
        tmpi = get_min_value(min_y*rate, quality);
        tmpi = decrease_min_value(tmpi, quality, tables->list_font_sizes[list_index].rotated_min_y*rate);
        min_y = (float)(tmpi-quality);

        ret = glyph_drawer_init(&font_draw, (int)(max_x-min_x), (int)(max_y-min_y));
        if (ret) {
            return ret;
        }

        font_draw.line_min_x = 0;
        font_draw.line_min_y = 0;
        font_draw.line_max_x = 0;
        font_draw.line_max_y = 0;

        for (i2=0;i2<tables->list_glyph[i].list_path_size;i2++) {
            for (i3=0;i3<tables->list_glyph[i].list_path[i2].list_glyph_curve_size;i3++) {
                rotate_by_angle_zero(&rotated_x[0], &rotated_y[0],
                                    tables->list_glyph[i].list_path[i2].list_glyph_curve[i3].x0*rate,
                                    tables->list_glyph[i].list_path[i2].list_glyph_curve[i3].y0*rate,
                                    rotate);
                rotate_by_angle_zero(&rotated_x[1], &rotated_y[1],
                                    tables->list_glyph[i].list_path[i2].list_glyph_curve[i3].x1*rate,
                                    tables->list_glyph[i].list_path[i2].list_glyph_curve[i3].y1*rate,
                                    rotate);
                if (tables->list_glyph[i].list_path[i2].list_glyph_curve[i3].is_curve == 1) {
                    rotate_by_angle_zero(&rotated_x[2], &rotated_y[2],
                                    tables->list_glyph[i].list_path[i2].list_glyph_curve[i3].curve_x*rate,
                                    tables->list_glyph[i].list_path[i2].list_glyph_curve[i3].curve_y*rate,
                                    rotate);
                    glyph_drawer_paint_curve(rotated_x[0]-min_x+move_glyph_x,
                                             rotated_y[0]-min_y+move_glyph_y,
                                             rotated_x[1]-min_x+move_glyph_x,
                                             rotated_y[1]-min_y+move_glyph_y,
                                             rotated_x[2]-min_x+move_glyph_x,
                                             rotated_y[2]-min_y+move_glyph_y,
                                             &font_draw, line__draw_index++);
                } else {
                    glyph_drawer_draw_line((int)(rotated_x[0]-min_x+move_glyph_x),
                                           (int)(rotated_y[0]-min_y+move_glyph_y),
                                           (int)(rotated_x[1]-min_x+move_glyph_x),
                                           (int)(rotated_y[1]-min_y+move_glyph_y),
                                           &font_draw, line__draw_index++);
                }
            }
        }

        glyph_filler_draw_inner_area(&font_draw);
        image_data->list_data[list_index].character = tables->corr_character_table.character[i];

        ret = glyph_image_add_glyph_into_image(tables->list_font_sizes, list_index,
                                               &font_draw, quality, image_data,
                                               (int32_t)(-min_x),
                                               (int32_t)(-min_y));
        if (ret) {
            return ret;
        }

        image_data->list_data[list_index].image_pixel_advance_x = hmtx_get_advance(i, hor_metrics_table,
                                                                                      hor_header_table,
                                                                                      font_size_px/(float)tables->header_table.units_per_em);
        image_data->list_data[list_index].image_pixel_bearing = hmtx_get_bearing(i, hor_metrics_table,
                                                                                      hor_header_table,
                                                                                      font_size_px/(float)tables->header_table.units_per_em);
        list_index++;
        glyph_drawer_clear(&font_draw);
    }
    return 0;
}
#endif // #ifndef TEST_CASE
