/*!
 * \file
 * \brief file head.c
 *
 * Font header table
 * https://docs.microsoft.com/en-us/typography/opentype/spec/head
 *
 * Copyright of Timo Hannukkala. All rights reserved.
 *
 * \author Timo Hannukkala <timohannukkala@hotmail.com>
 */

#include "head.h"
#include <string.h>
#include "parse_value.h"

/*!
 * \brief head_parse_head_table
 *
 * Parse font header table
 * https://docs.microsoft.com/en-us/typography/opentype/spec/head
 *
 * \param data
 * \param data_size
 * \param offset
 * \param header_table
 * \return 0 on success
 */
int head_parse_head_table(const uint8_t* data, size_t data_size, size_t offset, font_header_table_t *header_table)
{
    int ret;
    ret = parse_value_16u(data, data_size, &offset, &header_table->major_version);
    if (ret) {
        return ret;
    }
    ret = parse_value_16u(data, data_size, &offset, &header_table->minor_version);
    if (ret) {
        return ret;
    }
    ret = parse_value_32f(data, data_size, &offset, &header_table->font_revision);
    if (ret) {
        return ret;
    }
    ret = parse_value_32u(data, data_size, &offset, &header_table->check_sum_adjustment);
    if (ret) {
        return ret;
    }
    ret = parse_value_32u(data, data_size, &offset, &header_table->magic_number);
    if (ret) {
        return ret;
    }
    ret = parse_value_16u(data, data_size, &offset, &header_table->flags);
    if (ret) {
        return ret;
    }
    ret = parse_value_16u(data, data_size, &offset, &header_table->units_per_em);
    if (ret) {
        return ret;
    }
    ret = parse_value_64i(data, data_size, &offset, &header_table->created);
    if (ret) {
        return ret;
    }
    ret = parse_value_64i(data, data_size, &offset, &header_table->modified);
    if (ret) {
        return ret;
    }
    ret = parse_value_16i(data, data_size, &offset, &header_table->x_min);
    if (ret) {
        return ret;
    }
    ret = parse_value_16i(data, data_size, &offset, &header_table->y_min);
    if (ret) {
        return ret;
    }
    ret = parse_value_16i(data, data_size, &offset, &header_table->x_max);
    if (ret) {
        return ret;
    }
    ret = parse_value_16i(data, data_size, &offset, &header_table->y_max);
    if (ret) {
        return ret;
    }
    ret = parse_value_16u(data, data_size, &offset, &header_table->font_style);
    if (ret) {
        return ret;
    }
    ret = parse_value_16u(data, data_size, &offset, &header_table->lowest_rec_ppem);
    if (ret) {
        return ret;
    }
    ret = parse_value_16i(data, data_size, &offset, &header_table->font_direction_hint);
    if (ret) {
        return ret;
    }
    ret = parse_value_16i(data, data_size, &offset, &header_table->format);
    if (ret) {
        return ret;
    }
    ret = parse_value_16i(data, data_size, &offset, &header_table->glyph_data_format);
    if (ret) {
        return ret;
    }
    return 0;
}
