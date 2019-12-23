/*!
 * \file
 * \brief file head.h
 *
 * Font header table
 * https://docs.microsoft.com/en-us/typography/opentype/spec/head
 *
 * Copyright of Timo Hannukkala. All rights reserved.
 *
 * \author Timo Hannukkala <timo.hannukkala@hotmail.com>
 */

#ifndef HEAD_H
#define HEAD_H

#include <stddef.h>
#include <stdint.h>
/*!
 * \brief The font_header_table_t struct
 * https://docs.microsoft.com/en-us/typography/opentype/spec/head
 */
typedef struct
{
    uint16_t major_version;
    uint16_t minor_version;
    float font_revision;
    uint32_t check_sum_adjustment;
    uint32_t magic_number;
    uint16_t flags;
    uint16_t units_per_em;
    int64_t created;
    int64_t modified;
    int16_t x_min;
    int16_t y_min;
    int16_t x_max;
    int16_t y_max;
    uint16_t font_style;
    uint16_t lowest_rec_ppem;
    int16_t font_direction_hint;
    int16_t format;
    int16_t glyph_data_format;
} font_header_table_t;

int head_parse_head_table(const uint8_t* data, size_t data_size, size_t offset, font_header_table_t *header_table);

#endif // HEAD_H
