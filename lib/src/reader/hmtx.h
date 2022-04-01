/*!
 * \file
 * \brief file hmtx.h
 *
 * Horizontal Metrics Table
 * https://docs.microsoft.com/en-us/typography/opentype/spec/hmtx
 *
 * Copyright of Timo Hannukkala. All rights reserved.
 *
 * \author Timo Hannukkala <timohannukkala@hotmail.com>
 */

#ifndef HMTX_H
#define HMTX_H

#include <stdint.h>
#include <stddef.h>
#include "hhea.h"
#include "maxp.h"

/*!
 * \brief The long_hor_metric_t struct
 *
 * LongHorMetric
 *
 * https://docs.microsoft.com/en-us/typography/opentype/spec/hmtx
 */
typedef struct
{
    uint16_t advance_width;
    int16_t left_side_bearing;
} long_hor_metric_t;

typedef struct
{
    long_hor_metric_t *list_long_hor_metric;
    int16_t *list_left_side_bearing;
} horizontal_metrics_table_t;

int hmtx_parse(const uint8_t *data, size_t data_size,
               horizontal_metrics_table_t *table, size_t hmtx_offset,
               const horizontal_header_table_t *hor_header_table,
               const maximum_profile_t *max_profile);
void hmtx_clear(horizontal_metrics_table_t *list_long_hor_metric);

float hmtx_get_advance(uint16_t glyph_index,
                    const horizontal_metrics_table_t *table,
                    const horizontal_header_table_t *hor_header_table,
                    float rate);
float hmtx_get_bearing(uint16_t glyph_index,
                    const horizontal_metrics_table_t *table,
                    const horizontal_header_table_t *hor_header_table,
                    float rate);


#endif // HMTX_H
