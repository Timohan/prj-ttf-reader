/*!
 * \file
 * \brief file glyf.h
 *
 * glyph Data
 * https://docs.microsoft.com/en-us/typography/opentype/spec/glyf
 *
 * Copyright of Timo Hannukkala. All rights reserved.
 *
 * \author Timo Hannukkala <timo.hannukkala@hotmail.com>
 */

#ifndef GLYF_H
#define GLYF_H

#include <stdint.h>
#include <stddef.h>
#include "loca.h"
#include "maxp.h"

/*!
 * \brief The glyph_curve_t struct
 *
 * Lines and curves of the glyph "line"
 */
typedef struct {
    float x0, y0;
    float x1, y1;
    float curve_x, curve_y; // curve points, only used when is_curve == 1
    uint8_t is_curve : 1;
} glyph_curve_t;

/*!
 * \brief The glyph_path_t struct
 *
 * paths of the curves and lines
 */
typedef struct {
    glyph_curve_t *list_glyph_curve;
    uint32_t list_glyph_curve_size;
} glyph_path_t;

/*!
 * \brief The glyph_t struct
 *
 * glyph struct, this contains all
 *
 */
typedef struct {
    uint16_t glyph_index;
    glyph_path_t *list_path;
    uint32_t list_path_size;
    int16_t min_x;
    int16_t min_y;
    int16_t max_x;
    int16_t max_y;
    int16_t num_contours;
    uint16_t *list_end_points_contours;   // list size = num_contours
    uint16_t max_num_end_contours;    // biggest number of list_end_points_contours
    uint16_t instruction_size;
    uint8_t *list_instructions;
    uint8_t  *list_flags;   // list size = glyph.max_num_end_contours + 1
    int16_t *list_point_x;  // list size = glyph.max_num_end_contours + 1
    int16_t *list_point_y;  // list size = glyph.max_num_end_contours + 1
    uint8_t loaded : 1;
} glyph_t;

/*!
 * These defines are used in flags of glyf
 * https://docs.microsoft.com/en-us/typography/opentype/spec/glyf
 */
#define ON_CURVE_POINT                          0x01
#define X_SHORT_VECTOR                          0x02
#define Y_SHORT_VECTOR                          0x04
#define REPEAT_FLAG                             0x08
#define X_IS_SAME_OR_POSITIVE_X_SHORT_VECTOR    0x10
#define Y_IS_SAME_OR_POSITIVE_Y_SHORT_VECTOR    0x20
#define OVERLAP_SIMPLE                          0x40

#define ARG_1_AND_2_ARE_WORDS                   0x0001
#define ARGS_ARE_XY_VALUES                      0x0002
#define ROUND_XY_TO_GRID                        0x0004
#define WE_HAVE_A_SCALE                         0x0008
#define MORE_COMPONENTS                         0x0020
#define WE_HAVE_AN_X_AND_Y_SCALE                0x0040
#define WE_HAVE_A_TWO_BY_TWO                    0x0080
#define WE_HAVE_INSTRUCTIONS                    0x0100
#define USE_MY_METRICS                          0x0200
#define OVERLAP_COMPOUND                        0x0400
#define SCALED_COMPONENT_OFFSET                 0x0800
#define UNSCALED_COMPONENT_OFFSET               0x1000

int glyf_parse(const uint8_t *data, size_t data_size, uint16_t index_glyph,
               glyph_t *list_glyph, size_t glyf_offset,
               index_to_loc_table_t *list_index_to_loc_table, const maximum_profile_t *profile);
int glyf_alloc(glyph_t **list_glyph, const maximum_profile_t *profile);
void glyf_clear(glyph_t **list_glyph, const maximum_profile_t *profile);

#endif // GLYF_H
