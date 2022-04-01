/*!
 * \file
 * \brief file glyf.c
 *
 * glyph Data
 * https://docs.microsoft.com/en-us/typography/opentype/spec/glyf
 *
 * Copyright of Timo Hannukkala. All rights reserved.
 *
 * \author Timo Hannukkala <timohannukkala@hotmail.com>
 */

#include "glyf.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "parse_value.h"
#include "hmtx.h"
#include "cmap.h"
#include "maxp.h"
#include "loca.h"

int glyf_parse_point(const uint8_t *data, size_t data_size, uint8_t *listFlags, int16_t *listPoint, uint16_t num_points, size_t *offset, uint8_t x);

/*!
 * \brief glyf_is_off_curve
 *
 * is it curve or line
 *
 * \param flag
 * \return
 */
static uint8_t glyf_is_off_curve(uint8_t flag)
{
    if ((flag & ON_CURVE_POINT) == 0) {
        return 1;
    }

    return 0;
}

/*!
 * \brief f_2_dot_14
 *
 * https://docs.microsoft.com/en-us/typography/opentype/spec/otff
 * F2DOT14	16-bit signed fixed number with the low 14 bits of fraction (2.14).
 *
 * \param value
 * \return
 */
static float f_2_dot_14(int16_t value)
{
    const int16_t add = (int16_t)(value >> 14);
    const float divisor = 16384; // float(1 << 14);
    const float dividend = (float)(value & 0x3fff);
    return dividend/divisor+(float)add;
}

/*!
 * \brief glyf_generate_contours
 *
 * Generates glyphs contours
 *
 * \param glyph
 * \return 0 on success
 */
static int glyf_generate_contours(glyph_t *glyph)
{
    uint16_t i, index_contour;
    uint16_t num_points;
    float prev_point_x = 0;
    float prev_point_y = 0;
    int16_t x0;
    int16_t y0;
    int16_t x1;
    int16_t y1;
    uint16_t point_index0;
    uint16_t point_index1;
    uint16_t point_index2;
    uint16_t point_index_last;
    uint16_t num_points_per_contour;
    uint16_t point_index_0;
    uint16_t coutour_count_first_point = 0;
    uint16_t current_contour_index = 0;
    uint16_t num_points_list;
    uint8_t is_curve0;
    uint8_t is_curve1;
    glyph_curve_t curve;
    uint16_t **point_index;
    uint16_t *listPointsPerContour;
    uint16_t *contour_index;
    glyph_curve_t *tmp;
    const uint16_t u_num_contour = (uint16_t)glyph->num_contours;

    point_index = (uint16_t **)(malloc((uint32_t)(u_num_contour*sizeof(uint16_t *))));
    if (!point_index) {
        return errno;
    }
    listPointsPerContour = (uint16_t *)malloc( (uint32_t)(sizeof(uint16_t)*u_num_contour));
    if (!listPointsPerContour) {
        return errno;
    }

    for (i=0;i<glyph->num_contours;i++) {
        if (i == 0) {
            num_points = (uint16_t)(glyph->list_end_points_contours[i] + 1);
        } else {
            num_points = (uint16_t)(glyph->list_end_points_contours[i] - glyph->list_end_points_contours[i-1]);
        }

        point_index[i] = (uint16_t *)(malloc((uint32_t)(num_points*sizeof(uint16_t))));
        if (!point_index[i]) {
            return errno;
        }
        listPointsPerContour[i] = num_points;
    }

    num_points_list = (uint16_t)(glyph->max_num_end_contours + 1);

    glyph->list_path = (glyph_path_t *)malloc( (uint32_t)(sizeof(glyph_path_t)*u_num_contour) );
    if (!glyph->list_path) {
        return errno;
    }
    glyph->list_path_size = (uint32_t)glyph->num_contours;

    contour_index = (uint16_t *)malloc( (uint32_t)(sizeof(uint16_t)*num_points_list) );
    if (!contour_index) {
        return errno;
    }

    for (i=0;i<num_points_list;i++) {
        if (i > glyph->list_end_points_contours[current_contour_index]) {
            current_contour_index++;
            coutour_count_first_point = 0;
        }

        contour_index[i] = current_contour_index;
        point_index[current_contour_index][coutour_count_first_point] = i;
        coutour_count_first_point++;
    }

    for (i=0;i<glyph->num_contours;i++) {
        num_points_per_contour = listPointsPerContour[i];
        point_index_0 = point_index[i][0];

        //If the first point is off curve
        if (glyf_is_off_curve(glyph->list_flags[point_index_0])) {
            point_index_last = point_index[i][num_points_per_contour - 1];
            if (glyf_is_off_curve(glyph->list_flags[point_index_last])) {
                prev_point_x = (float)(glyph->list_point_x[point_index_0]+glyph->list_point_x[point_index_last])/2.0f;
                prev_point_y = (float)(glyph->list_point_y[point_index_0]+glyph->list_point_y[point_index_last])/2.0f;
            }
            else {
                prev_point_x = (float)glyph->list_point_x[point_index_last];
                prev_point_y = (float)glyph->list_point_y[point_index_last];
            }
        }

        for (index_contour=0;index_contour<num_points_per_contour;index_contour++) {
            point_index0 = point_index[i][index_contour % num_points_per_contour];
            point_index1 = point_index[i][(index_contour + 1) % num_points_per_contour];

            is_curve0 = glyf_is_off_curve(glyph->list_flags[point_index0]);
            is_curve1 = glyf_is_off_curve(glyph->list_flags[point_index1]);

            curve.is_curve = (is_curve0 || is_curve1);

            x0 = glyph->list_point_x[point_index0];
            y0 = glyph->list_point_y[point_index0];
            x1 = glyph->list_point_x[point_index1];
            y1 = glyph->list_point_y[point_index1];

            if (is_curve0) {
                curve.x0 = prev_point_x;
                curve.y0 = prev_point_y;
                curve.curve_x = x0;
                curve.curve_y = y0;

                if (is_curve1) {
                    curve.x1 = (float)(x0 + x1)/2.0f;
                    curve.y1 = (float)(y0 + y1)/2.0f;
                    prev_point_x = curve.x1;
                    prev_point_y = curve.y1;
                } else {
                    curve.x1 = x1;
                    curve.y1 = y1;
                }
            }  else if (is_curve1) {
                point_index2 = point_index[i][(index_contour + 2) % num_points_per_contour];

                if (glyf_is_off_curve(glyph->list_flags[point_index2])) {
                    curve.x0 = x0;
                    curve.y0 = y0;
                    curve.curve_x = x1;
                    curve.curve_y = y1;
                    curve.x1 = (float)(x1 + glyph->list_point_x[point_index2])/2.0f;
                    curve.y1 = (float)(y1 + glyph->list_point_y[point_index2])/2.0f;
                    prev_point_x = curve.x1;
                    prev_point_y = curve.y1;
                } else {
                    curve.x0 = x0;
                    curve.y0 = y0;
                    curve.curve_x = x1;
                    curve.curve_y = y1;
                    curve.x1 = glyph->list_point_x[point_index2];
                    curve.y1 = glyph->list_point_y[point_index2];
                    prev_point_x = x0;
                    prev_point_y = y0;
               }
            } else {
                curve.x0 = x0;
                curve.y0 = y0;
                curve.x1 = x1;
                curve.y1 = y1;
                // it's line, curve points are ignored
                curve.curve_x = 0;
                curve.curve_y = 0;

                prev_point_x = x0;
                prev_point_y = y0;
            }

            if (index_contour == 0) {
                glyph->list_path[i].list_glyph_curve_size = 1;
                glyph->list_path[i].list_glyph_curve = (glyph_curve_t *)malloc( sizeof(glyph_curve_t) );
                if (!glyph->list_path[i].list_glyph_curve) {
                    return errno;
                }
            } else {
                glyph->list_path[i].list_glyph_curve_size++;
                tmp = (glyph_curve_t *)realloc(glyph->list_path[i].list_glyph_curve, sizeof(glyph_curve_t)*glyph->list_path[i].list_glyph_curve_size );
                if (!tmp) {
                    return errno;
                }
                glyph->list_path[i].list_glyph_curve = tmp;
            }
            glyph->list_path[i].list_glyph_curve[glyph->list_path[i].list_glyph_curve_size - 1] = curve;
        }
    }

    for (i=0;i<glyph->num_contours;i++) {
        free(point_index[i]);
    }
    free(point_index);
    free(contour_index);
    free(listPointsPerContour);
    return 0;
}

/*!
 * \brief glyf_generate_glyph_from_transformation
 *
 * Calculates the glyph curve's positions
 * This is used in glyphs that are using other's glyps compositons but
 * with different size.
 * For example, "a" and "å", the "a" part is same shape but different position
 *
 * \param transformation
 * \param in curve to transformation
 * \param ret new glyph curve here
 */
static void glyf_generate_glyph_from_transformation(float transformation[6], const glyph_curve_t *in, glyph_curve_t *ret)
{
    ret->is_curve = in->is_curve;

    ret->x0 = in->x0*transformation[0] + in->y0*transformation[1] + transformation[4];
    ret->y0 = in->x0*transformation[2] + in->y0*transformation[3] + transformation[5];

    ret->x1 = in->x1 * transformation[0] + in->y1*transformation[1] + transformation[4];
    ret->y1 = in->x1 * transformation[2] + in->y1*transformation[3] + transformation[5];

    if (ret->is_curve) {
        ret->curve_x = in->curve_x*transformation[0] + in->curve_y*transformation[1] + transformation[4];
        ret->curve_y = in->curve_x*transformation[2] + in->curve_y*transformation[3] + transformation[5];
    } else {
        // line -> curve points are ignored
        ret->curve_x = 0;
        ret->curve_y = 0;
    }
}

/*!
 * \brief glyf_generate_compositive_glyph
 *
 * Generates composite glyph such as "ä"
 *
 * \param data
 * \param data_size
 * \param index_glyph
 * \param list_glyph
 * \param glyf_offset
 * \param list_index_to_loc_table
 * \param profile
 * \param offset
 * \return 0 on success
 */
static int glyf_generate_compositive_glyph(const uint8_t *data, size_t data_size, uint16_t index_glyph,
               glyph_t *list_glyph, size_t glyf_offset,
               index_to_loc_table_t *list_index_to_loc_table, const maximum_profile_t *profile, size_t offset)
{
    int16_t i;
    int j;
    int ret;
    float transformation[6];
    uint16_t index_composite_glyph;
    uint32_t index_composite_path;
    uint32_t index_composite_curve;
    uint16_t composite_glyph_flags;
    int16_t args_i16[2];
    int8_t args_i8[2];
    uint8_t is_word_value;
    int16_t tmp_value;
    uint16_t instructions_size;
    uint8_t composite_equal;
    glyph_path_t *tmp;

    for (i=0;i<-list_glyph[index_glyph].num_contours;i++) {
        while (1) {
            ret = parse_value_16u(data, data_size, &offset, &composite_glyph_flags);
            if (ret) {
                return ret;
            }

            ret = parse_value_16u(data, data_size, &offset, &index_composite_glyph);
            if (ret) {
                return ret;
            }

            if (composite_glyph_flags & ARG_1_AND_2_ARE_WORDS) {
                for (j=0;j<2;j++) {
                    ret = parse_value_16i(data, data_size, &offset, &args_i16[j]);
                    if (ret) {
                        return ret;
                    }
                }
                is_word_value = 1;

            } else {
                for (j=0;j<2;j++) {
                    ret = parse_value_8i(data, data_size, &offset, &args_i8[j]);
                    if (ret) {
                        return ret;
                    }
                }
                is_word_value = 0;
            }

            transformation[0] = 1.0f;
            transformation[1] = 0.0f;
            transformation[2] = 0.0f;
            transformation[3] = 1.0f;
            transformation[4] = 0.0f;
            transformation[5] = 0.0f;
            composite_equal = 1;

            if (composite_glyph_flags & WE_HAVE_A_SCALE) {
                ret = parse_value_16i(data, data_size, &offset, &tmp_value);
                if (ret) {
                    return ret;
                }
                transformation[0] = f_2_dot_14(tmp_value);
                transformation[3] = f_2_dot_14(tmp_value);
                composite_equal = 0;
            } else if (composite_glyph_flags & WE_HAVE_AN_X_AND_Y_SCALE) {
                ret = parse_value_16i(data, data_size, &offset, &tmp_value);
                if (ret) {
                    return ret;
                }
                transformation[0] = f_2_dot_14(tmp_value);
                ret = parse_value_16i(data, data_size, &offset, &tmp_value);
                if (ret) {
                    return ret;
                }
                transformation[3] = f_2_dot_14(tmp_value);
                composite_equal = 0;
            } else if (composite_glyph_flags & WE_HAVE_A_TWO_BY_TWO) {
                for (j=0;j<4;j++) {
                    ret = parse_value_16i(data, data_size, &offset, &tmp_value);
                    if (ret) {
                        return ret;
                    }
                    transformation[j] = f_2_dot_14(tmp_value);
                }
                composite_equal = 0;
            }

            if (composite_glyph_flags & ARGS_ARE_XY_VALUES) {
                composite_equal = 0;
                transformation[4] = is_word_value ? args_i16[0] : args_i8[0];
                transformation[5] = is_word_value ? args_i16[1] : args_i8[1];
                if (composite_glyph_flags & SCALED_COMPONENT_OFFSET) {
                    transformation[4] *= transformation[0];
                    transformation[5] *= transformation[3];
                }
            }

            //Skip instructions
            if (composite_glyph_flags & WE_HAVE_INSTRUCTIONS) {
                instructions_size = 0;
                ret = parse_value_16u(data, data_size, &offset, &instructions_size);
                if (ret) {
                    return ret;
                }
                offset += instructions_size;
            }

            if (list_glyph[index_composite_glyph].loaded == 0) {
                ret = glyf_parse(data, data_size, index_composite_glyph,
                               list_glyph, glyf_offset,
                               list_index_to_loc_table, profile);
                if (ret) {
                    return ret;
                }
            }

            for (index_composite_path=0;index_composite_path<list_glyph[index_composite_glyph].list_path_size;index_composite_path++) {
                list_glyph[index_glyph].list_path_size++;
                tmp = (glyph_path_t *)realloc(list_glyph[index_glyph].list_path, sizeof(glyph_path_t)*list_glyph[index_glyph].list_path_size );
                if (!tmp) {
                    return errno;
                }
                list_glyph[index_glyph].list_path = tmp;

                list_glyph[index_glyph].list_path[ list_glyph[index_glyph].list_path_size - 1 ].list_glyph_curve = (glyph_curve_t *)malloc( sizeof(glyph_curve_t)*list_glyph[index_composite_glyph].list_path[index_composite_path].list_glyph_curve_size);
                if (!list_glyph[index_glyph].list_path[ list_glyph[index_glyph].list_path_size - 1 ].list_glyph_curve) {
                    return errno;
                }
                list_glyph[index_glyph].list_path[ list_glyph[index_glyph].list_path_size - 1 ].list_glyph_curve_size = list_glyph[index_composite_glyph].list_path[index_composite_path].list_glyph_curve_size;

                if (composite_equal == 0) {
                    for (index_composite_curve=0;index_composite_curve<list_glyph[index_composite_glyph].list_path[index_composite_path].list_glyph_curve_size;index_composite_curve++) {
                        glyf_generate_glyph_from_transformation(transformation,
                                                                &list_glyph[index_composite_glyph].list_path[index_composite_path].list_glyph_curve[index_composite_curve],
                                                                &list_glyph[index_glyph].list_path[ list_glyph[index_glyph].list_path_size - 1 ].list_glyph_curve[index_composite_curve]);
                    }
                } else {
                    for (index_composite_curve= 0;index_composite_curve<list_glyph[index_composite_glyph].list_path[index_composite_path].list_glyph_curve_size;index_composite_curve++) {
                        list_glyph[index_glyph].list_path[ list_glyph[index_glyph].list_path_size - 1 ].list_glyph_curve[index_composite_curve] = list_glyph[index_composite_glyph].list_path[index_composite_path].list_glyph_curve[index_composite_curve];
                    }
                }
            }

            if ((composite_glyph_flags & MORE_COMPONENTS) == 0) {
                break;
            }
        }
    }

    return 0;
}

/*!
 * \brief glyf_parse
 *
 * https://docs.microsoft.com/en-us/typography/opentype/spec/glyf
 *
 * \param data
 * \param data_size
 * \param index_glyph this is index of glyph on the table
 * \param list_glyph
 * \param glyf_offset
 * \param list_index_to_loc_table
 * \param profile
 * \return
 */
int glyf_parse(const uint8_t *data, size_t data_size, uint16_t index_glyph,
               glyph_t *list_glyph, size_t glyf_offset,
               index_to_loc_table_t *list_index_to_loc_table, const maximum_profile_t *profile) {
    int ret;
    int16_t i;
    uint16_t num_points;
    uint8_t repeat;

    if (list_glyph[index_glyph].loaded) {
        return 0;
    }

    glyph_t *glyph = &list_glyph[index_glyph];
    glyph->glyph_index = index_glyph;

    glyph->list_path = NULL;
    glyph->list_path_size = 0;

    size_t offset = glyf_offset + (size_t)list_index_to_loc_table[index_glyph].offset;

    ret = parse_value_16i(data, data_size, &offset, &glyph->num_contours);
    if (ret) {
        return ret;
    }

    ret = parse_value_16i(data, data_size, &offset, &glyph->min_x);
    if (ret) {
        return ret;
    }

    ret = parse_value_16i(data, data_size, &offset, &glyph->min_y);
    if (ret) {
        return ret;
    }

    ret = parse_value_16i(data, data_size, &offset, &glyph->max_x);
    if (ret) {
        return ret;
    }

    ret = parse_value_16i(data, data_size, &offset, &glyph->max_y);
    if (ret) {
        return ret;
    }

    glyph->loaded = 1;

    if (glyph->num_contours > 0) {
        const uint16_t u_num_contours = (uint16_t)glyph->num_contours;
        glyph->max_num_end_contours = 0;
        glyph->list_end_points_contours = (uint16_t *)malloc((uint32_t)(sizeof(uint16_t)*u_num_contours));
        if (!glyph->list_end_points_contours) {
            return errno;
        }
        for (i=0;i<glyph->num_contours;i++) {
            ret = parse_value_16u(data, data_size, &offset, &glyph->list_end_points_contours[i]);
            if (ret) {
                return ret;
            }
            if (glyph->max_num_end_contours < glyph->list_end_points_contours[i]) {
                glyph->max_num_end_contours = glyph->list_end_points_contours[i];
            }
        }

        ret = parse_value_16u(data, data_size, &offset, &glyph->instruction_size);
        if (ret) {
            return ret;
        }

        if (glyph->instruction_size) {
            if (offset + glyph->instruction_size >= data_size) {
                return EIO;
            }

            glyph->list_instructions = (uint8_t *)malloc(sizeof(uint8_t)*glyph->instruction_size);
            if (!glyph->list_instructions) {
                return errno;
            }
            memcpy(glyph->list_instructions, data+offset, glyph->instruction_size);
            offset += glyph->instruction_size;
        }

        num_points = (uint16_t)(glyph->max_num_end_contours + 1);
        repeat = 0;

        glyph->list_flags = (uint8_t *)malloc(sizeof(uint8_t)*num_points);
        if (!glyph->list_flags) {
            return errno;
        }
        glyph->list_point_x = (int16_t *)malloc(sizeof(int16_t)*num_points);
        if (!glyph->list_point_x) {
            return errno;
        }
        glyph->list_point_y = (int16_t *)malloc(sizeof(int16_t)*num_points);
        if (!glyph->list_point_y) {
            return errno;
        }

        for (i=0;i<num_points;i++) {
            if (repeat == 0) {
                glyph->list_flags[i] = data[offset];
                offset++;
                if (glyph->list_flags[i] & REPEAT_FLAG) {
                    repeat = data[offset];
                    offset++;
                }
            } else {
                glyph->list_flags[i] = glyph->list_flags[i-1];
                repeat--;
            }
        }

        ret = glyf_parse_point(data, data_size, glyph->list_flags, glyph->list_point_x, num_points, &offset, 1);
        if (ret) {
            return ret;
        }

        ret = glyf_parse_point(data, data_size, glyph->list_flags, glyph->list_point_y, num_points, &offset, 0);
        if (ret) {
            return ret;
        }

        return glyf_generate_contours(glyph);
    }

    // number of contours is negative (or 0), this means
    // it's composite glyph -> it's composite glyph, such as "ä"
    // which contains a and two dots
    return glyf_generate_compositive_glyph(data, data_size, index_glyph,
                                           list_glyph, glyf_offset,
                                           list_index_to_loc_table, profile, offset);
}

/*!
 * \brief glyf_parse_point
 *
 * parse x or y point?
 *
 * \param data
 * \param data_size
 * \param listFlags list of flags
 * \param listPoint list of points (x or y)
 * \param num_points number of points (or flags)
 * \param offset
 * \param x 1 == x, 0 == y
 * \return 0 == success
 */
int glyf_parse_point(const uint8_t *data, size_t data_size, uint8_t *listFlags, int16_t *listPoint, uint16_t num_points, size_t *offset, uint8_t x)
{
    int ret;
    int16_t i;
    uint8_t is_same_or_positive_short_vector = x ? X_IS_SAME_OR_POSITIVE_X_SHORT_VECTOR : Y_IS_SAME_OR_POSITIVE_Y_SHORT_VECTOR;
    uint8_t short_vector = x ? X_SHORT_VECTOR : Y_SHORT_VECTOR;

    for (i=0;i<num_points;i++) {
        if (listFlags[i] & is_same_or_positive_short_vector
                && (listFlags[i] & short_vector) == 0) {
            if (i > 0) {
                listPoint[i] = listPoint[i-1];
            } else {
                listPoint[i] = 0;
            }
        } else {
            if (listFlags[i] & short_vector) {
                if (*offset >= data_size) {
                    return EIO;
                }
                listPoint[i] = data[*offset];
                *offset += 1;
                if ((listFlags[i] & is_same_or_positive_short_vector) == 0) {
                    listPoint[i] = (int16_t)(listPoint[i]*-1);
                }
            } else {
                ret = parse_value_16i(data, data_size, offset, &listPoint[i]);
                if (ret) {
                    return ret;
                }
            }

            if (i != 0) {
                listPoint[i] = (int16_t)(listPoint[i-1] + listPoint[i]);
            }
        }
    }

    return 0;
}

/*!
 * \brief glyf_alloc
 *
 * alloc the list of glyphs
 *
 * \param list_glyph
 * \param profile
 * \return 0 on success
 */
int glyf_alloc(glyph_t **list_glyph, const maximum_profile_t *profile)
{
    *list_glyph = (glyph_t *)malloc(sizeof(glyph_t)*profile->glyphs_count);
    if (!*list_glyph) {
        return errno;
    }
    memset(*list_glyph, 0, sizeof(glyph_t)*profile->glyphs_count);
    return 0;
}

/*!
 * \brief glyf_clear
 *
 * clears the list of glyphs
 *
 * \param list_glyph
 * \param profile is required for glyphs_count
 */
void glyf_clear(glyph_t **list_glyph, const maximum_profile_t *profile)
{
    uint16_t i;
    size_t i2;
    if (*list_glyph) {
        for (i=0;i<profile->glyphs_count;i++) {
            if ((*list_glyph)[i].list_end_points_contours) {
                free((*list_glyph)[i].list_end_points_contours);
            }

            if ((*list_glyph)[i].list_instructions) {
                free((*list_glyph)[i].list_instructions);
            }
            if ((*list_glyph)[i].list_flags) {
                free((*list_glyph)[i].list_flags);
            }
            if ((*list_glyph)[i].list_point_x) {
                free((*list_glyph)[i].list_point_x);
            }
            if ((*list_glyph)[i].list_point_y) {
                free((*list_glyph)[i].list_point_y);
            }

            for (i2=0;i2<(*list_glyph)[i].list_path_size;i2++) {
                free((*list_glyph)[i].list_path[i2].list_glyph_curve);
            }
            free((*list_glyph)[i].list_path);
        }
        free(*list_glyph);
        *list_glyph = NULL;
    }
}
