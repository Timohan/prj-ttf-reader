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

#include "hhea.h"
#include <string.h>
#include "parse_value.h"

/*!
 * \brief hhea_parse
 *
 * https://docs.microsoft.com/en-us/typography/opentype/spec/hhea
 *
 * \param data
 * \param data_size
 * \param offset
 * \param table
 * \return 0 on success
 */
int hhea_parse(const uint8_t* data, size_t data_size, size_t offset, horizontal_header_table_t *table)
{
    int ret;
    int i;

    ret = parse_value_16u(data, data_size, &offset, &table->major_version);
    if (ret) {
        return ret;
    }
    ret = parse_value_16u(data, data_size, &offset, &table->minor_version);
    if (ret) {
        return ret;
    }
    ret = parse_value_16i(data, data_size, &offset, &table->ascender);
    if (ret) {
        return ret;
    }
    ret = parse_value_16i(data, data_size, &offset, &table->descender);
    if (ret) {
        return ret;
    }
    ret = parse_value_16i(data, data_size, &offset, &table->line_gap);
    if (ret) {
        return ret;
    }
    ret = parse_value_16u(data, data_size, &offset, &table->max_advance_width);
    if (ret) {
        return ret;
    }
    ret = parse_value_16i(data, data_size, &offset, &table->min_left_side_bearing);
    if (ret) {
        return ret;
    }
    ret = parse_value_16i(data, data_size, &offset, &table->min_right_side_bearing);
    if (ret) {
        return ret;
    }
    ret = parse_value_16i(data, data_size, &offset, &table->x_max_extent);
    if (ret) {
        return ret;
    }
    ret = parse_value_16i(data, data_size, &offset, &table->caret_slope_rise);
    if (ret) {
        return ret;
    }
    ret = parse_value_16i(data, data_size, &offset, &table->caret_slope_run);
    if (ret) {
        return ret;
    }
    ret = parse_value_16i(data, data_size, &offset, &table->caret_offset);
    if (ret) {
        return ret;
    }
    for (i=0;i<4;i++) {
        ret = parse_value_16i(data, data_size, &offset, &table->reserved[i]);
        if (ret) {
            return ret;
        }
    }
    ret = parse_value_16i(data, data_size, &offset, &table->metric_data_format);
    if (ret) {
        return ret;
    }
    ret = parse_value_16u(data, data_size, &offset, &table->number_of_hmetrics);
    if (ret) {
        return ret;
    }

    return 0;
}
