/*!
 * \file
 * \brief file hmtx.c
 *
 * Horizontal Metrics Table
 * https://docs.microsoft.com/en-us/typography/opentype/spec/hmtx
 *
 * Copyright of Timo Hannukkala. All rights reserved.
 *
 * \author Timo Hannukkala <timo.hannukkala@hotmail.com>
 */

#include "hmtx.h"
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include "hhea.h"
#include "maxp.h"
#include "parse_value.h"

/*!
 * \brief hmtx_parse
 *
 * https://docs.microsoft.com/en-us/typography/opentype/spec/hmtx
 *
 * \param data
 * \param data_size
 * \param table
 * \param hmtx_offset
 * \param hor_header_table
 * \param max_profile
 * \return 0 on success
 */
int hmtx_parse(const uint8_t *data, size_t data_size,
               horizontal_metrics_table_t *table, size_t hmtx_offset,
               const horizontal_header_table_t *hor_header_table,
               const maximum_profile_t *max_profile)
{
    int ret = 0;
    uint16_t i;
    if (hor_header_table->number_of_hmetrics) {
        table->list_long_hor_metric = (long_hor_metric_t *)malloc(sizeof(long_hor_metric_t)*hor_header_table->number_of_hmetrics);
        for (i=0;i<hor_header_table->number_of_hmetrics;i++) {
            ret = parse_value_16u(data, data_size, &hmtx_offset, &table->list_long_hor_metric[i].advance_width);
            if (ret) {
                return ret;
            }

            ret = parse_value_16i(data, data_size, &hmtx_offset, &table->list_long_hor_metric[i].left_side_bearing);
            if (ret) {
                return ret;
            }
        }
    }

    if (max_profile->glyphs_count - hor_header_table->number_of_hmetrics > 0) {
        table->list_left_side_bearing = (int16_t *)malloc(sizeof(int16_t)*(uint16_t)(max_profile->glyphs_count - hor_header_table->number_of_hmetrics));
        for (i=0;i<max_profile->glyphs_count - hor_header_table->number_of_hmetrics;i++) {
            ret = parse_value_16i(data, data_size, &hmtx_offset, &table->list_left_side_bearing[i]);
            if (ret) {
                return ret;
            }
        }
    }
    return ret;
}

/*!
 * \brief hmtx_clear
 * \param list_long_hor_metric
 */
void hmtx_clear(horizontal_metrics_table_t *list_long_hor_metric)
{
    free(list_long_hor_metric->list_long_hor_metric);
    free(list_long_hor_metric->list_left_side_bearing);
}

/*!
 * \brief hmtx_get_advance
 *
 * get calculated advance for glyph by glyph_index
 *
 * \param glyph_index
 * \param table
 * \param hor_header_table
 * \param rate
 * \return calculated advance
 */
float hmtx_get_advance(uint16_t glyph_index,
                    const horizontal_metrics_table_t *table,
                    const horizontal_header_table_t *hor_header_table,
                    float rate)
{
    if (!hor_header_table->number_of_hmetrics) {
        return 0;
    }

    if (hor_header_table->number_of_hmetrics) {
        if (hor_header_table->number_of_hmetrics > glyph_index) {
            return (float)(rate*(float)table->list_long_hor_metric[glyph_index].advance_width);
        }
    }

    return 0;
}

/*!
 * \brief hmtx_get_bearing
 *
 * get calculated bearing for glyph by glyph_index
 *
 * \param glyph_index
 * \param table
 * \param hor_header_table
 * \param rate
 * \return calculated bearing
 */
float hmtx_get_bearing(uint16_t glyph_index,
                    const horizontal_metrics_table_t *table,
                    const horizontal_header_table_t *hor_header_table,
                    float rate)
{
    if (!hor_header_table->number_of_hmetrics) {
        return 0;
    }

    if (hor_header_table->number_of_hmetrics) {
        if (hor_header_table->number_of_hmetrics > glyph_index) {
            return (float)(rate*(float)table->list_long_hor_metric[glyph_index].left_side_bearing);
        }
    }

    return (float)(rate*(float)table->list_long_hor_metric[glyph_index-hor_header_table->number_of_hmetrics].left_side_bearing);
}
