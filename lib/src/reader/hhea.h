/*!
 * \file
 * \brief file hhea.c
 *
 * Horizontal Header Table
 * https://docs.microsoft.com/en-us/typography/opentype/spec/hhea
 *
 * Copyright of Timo Hannukkala. All rights reserved.
 *
 * \author Timo Hannukkala <timo.hannukkala@hotmail.com>
 */

#ifndef HHEA_H
#define HHEA_H

#include <stdint.h>
#include <stddef.h>
/*!
 * \brief The horizontal_header_table_t struct
 * https://docs.microsoft.com/en-us/typography/opentype/spec/hhea
 */
typedef struct {
    uint16_t major_version;
    uint16_t minor_version;
    int16_t	ascender;
    int16_t	descender;
    int16_t	line_gap;
    uint16_t max_advance_width;
    int16_t	min_left_side_bearing;
    int16_t	min_right_side_bearing;
    int16_t	x_max_extent;
    int16_t	caret_slope_rise;
    int16_t	caret_slope_run;
    int16_t	caret_offset;
    int16_t reserved[4];
    int16_t	metric_data_format;
    uint16_t number_of_hmetrics;
} horizontal_header_table_t;

int hhea_parse(const uint8_t* data, size_t data_size, size_t offset, horizontal_header_table_t *table);

#endif // HHEA_H
