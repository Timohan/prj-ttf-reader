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
int32_t get_min_value(float value, int quality) {
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

/*!
 * \brief get_zero_line_value
 * \param value
 * \param quality
 * \return
 */
#ifndef TEST_CASE
static
#endif
int32_t get_zero_line_value(float value, int quality)
{
    int i_value = (int)value;

    if (i_value % quality == 0 && fabs((float)i_value - value) <= 0) {
        return i_value/quality;
    }

    i_value -= i_value % quality;
    if (value >= 0) {
        return i_value/quality;
    }

    return i_value/quality - 1;
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
 * \return 0 == success
 */
int glyph_graph_generator_generate_graph(const uint32_t *list_characters, uint32_t list_characters_size,
                                         const uint8_t *data, size_t data_size, float font_size_px,
                                         font_tables_t *tables, int quality,
                                         prj_ttf_reader_data_t *image_data,
                                         const horizontal_metrics_table_t *hor_metrics_table,
                                         const horizontal_header_table_t *hor_header_table)
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
    float square_min_x;
    float square_min_y;
    float square_max_x;
    float square_max_y;
    int first_time;
    int is_empty;
    int ret;
    int32_t tmpi;
    font_size_t *tmp;
    font_drawing_t font_draw;

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

        square_min_x = 0;
        square_min_y = 0;
        square_max_x = 0;
        square_max_y = 0;
        first_time = 1;

        for (i2=0;i2<tables->list_glyph[i].list_path_size;i2++) {
            for (i3=0;i3<tables->list_glyph[i].list_path[i2].list_glyph_curve_size;i3++) {
                if (first_time) {
                    first_time = 0;
                    square_min_x = tables->list_glyph[i].list_path[i2].list_glyph_curve[i3].x0;
                    square_max_x = tables->list_glyph[i].list_path[i2].list_glyph_curve[i3].x0;
                    square_min_y = tables->list_glyph[i].list_path[i2].list_glyph_curve[i3].y0;
                    square_max_y = tables->list_glyph[i].list_path[i2].list_glyph_curve[i3].y0;
                } else {
                    if (square_min_x > tables->list_glyph[i].list_path[i2].list_glyph_curve[i3].x0) {
                        square_min_x = tables->list_glyph[i].list_path[i2].list_glyph_curve[i3].x0;
                    }
                    if (square_min_y > tables->list_glyph[i].list_path[i2].list_glyph_curve[i3].y0) {
                        square_min_y = tables->list_glyph[i].list_path[i2].list_glyph_curve[i3].y0;
                    }
                    if (square_max_x < tables->list_glyph[i].list_path[i2].list_glyph_curve[i3].x0) {
                        square_max_x = tables->list_glyph[i].list_path[i2].list_glyph_curve[i3].x0;
                    }
                    if (square_max_y < tables->list_glyph[i].list_path[i2].list_glyph_curve[i3].y0) {
                        square_max_y = tables->list_glyph[i].list_path[i2].list_glyph_curve[i3].y0;
                    }
                }

                if (square_min_x > tables->list_glyph[i].list_path[i2].list_glyph_curve[i3].x1) {
                    square_min_x = tables->list_glyph[i].list_path[i2].list_glyph_curve[i3].x1;
                }
                if (square_min_y > tables->list_glyph[i].list_path[i2].list_glyph_curve[i3].y1) {
                    square_min_y = tables->list_glyph[i].list_path[i2].list_glyph_curve[i3].y1;
                }
                if (square_max_x < tables->list_glyph[i].list_path[i2].list_glyph_curve[i3].x1) {
                    square_max_x = tables->list_glyph[i].list_path[i2].list_glyph_curve[i3].x1;
                }
                if (square_max_y < tables->list_glyph[i].list_path[i2].list_glyph_curve[i3].y1) {
                    square_max_y = tables->list_glyph[i].list_path[i2].list_glyph_curve[i3].y1;
                }

                if (!tables->list_glyph[i].list_path[i2].list_glyph_curve[i3].is_curve) {
                    // it's line, curve points are ignored
                    continue;
                }

                if (square_min_x > tables->list_glyph[i].list_path[i2].list_glyph_curve[i3].curve_x) {
                    square_min_x = tables->list_glyph[i].list_path[i2].list_glyph_curve[i3].curve_x;
                }
                if (square_min_y > tables->list_glyph[i].list_path[i2].list_glyph_curve[i3].curve_y) {
                    square_min_y = tables->list_glyph[i].list_path[i2].list_glyph_curve[i3].curve_y;
                }
                if (square_max_x < tables->list_glyph[i].list_path[i2].list_glyph_curve[i3].curve_x) {
                    square_max_x = tables->list_glyph[i].list_path[i2].list_glyph_curve[i3].curve_x;
                }
                if (square_max_y < tables->list_glyph[i].list_path[i2].list_glyph_curve[i3].curve_y) {
                    square_max_y = tables->list_glyph[i].list_path[i2].list_glyph_curve[i3].curve_y;
                }
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
        tables->list_font_sizes[tables->list_font_sizes_count].width = (int)( (float)(square_max_x-square_min_x)*rate/(float)quality) + 3;
        tables->list_font_sizes[tables->list_font_sizes_count].height = (int)( (float)(square_max_y-square_min_y)*rate/(float)quality) + 3;
        tables->list_font_sizes[tables->list_font_sizes_count].min_x = square_min_x;
        tables->list_font_sizes[tables->list_font_sizes_count].max_x = square_max_x;
        tables->list_font_sizes[tables->list_font_sizes_count].min_y = square_min_y;
        tables->list_font_sizes[tables->list_font_sizes_count].max_y = square_max_y;
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
        max_x = (float)tmpi;
        tmpi = get_max_value(max_y*rate, quality);
        max_y = (float)tmpi;
        tmpi = get_min_value(min_x*rate, quality);
        min_x = (float)tmpi;
        tmpi = get_min_value(min_y*rate, quality);
        min_y = (float)tmpi;

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
                if (tables->list_glyph[i].list_path[i2].list_glyph_curve[i3].is_curve == 1) {
                    glyph_drawer_paint_curve(tables->list_glyph[i].list_path[i2].list_glyph_curve[i3].x0*rate-min_x,
                                             tables->list_glyph[i].list_path[i2].list_glyph_curve[i3].y0*rate-min_y,
                                             tables->list_glyph[i].list_path[i2].list_glyph_curve[i3].x1*rate-min_x,
                                             tables->list_glyph[i].list_path[i2].list_glyph_curve[i3].y1*rate-min_y,
                                             tables->list_glyph[i].list_path[i2].list_glyph_curve[i3].curve_x*rate-min_x,
                                             tables->list_glyph[i].list_path[i2].list_glyph_curve[i3].curve_y*rate-min_y,
                                             &font_draw, line__draw_index++);
                } else {
                    glyph_drawer_draw_line((int)(tables->list_glyph[i].list_path[i2].list_glyph_curve[i3].x0*rate-min_x),
                                           (int)(tables->list_glyph[i].list_path[i2].list_glyph_curve[i3].y0*rate-min_y),
                                           (int)(tables->list_glyph[i].list_path[i2].list_glyph_curve[i3].x1*rate-min_x),
                                           (int)(tables->list_glyph[i].list_path[i2].list_glyph_curve[i3].y1*rate-min_y),
                                           &font_draw, line__draw_index++);
                }
            }
        }

        glyph_filler_draw_inner_area(&font_draw);
        image_data->list_data[list_index].character = tables->corr_character_table.character[i];
        ret = glyph_image_add_glyph_into_image(tables->list_font_sizes, list_index,
                                               &font_draw, quality, image_data,
                                               get_zero_line_value(tables->list_glyph[i].min_y*rate, quality));
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
