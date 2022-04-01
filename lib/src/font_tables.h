/*!
 * \file
 * \brief file font_tables.h
 *
 * Collector struct for parsed data to generate glyphs
 *
 * Copyright of Timo Hannukkala. All rights reserved.
 *
 * \author Timo Hannukkala <timohannukkala@hotmail.com>
 */

#ifndef FONT_TABLES_H
#define FONT_TABLES_H

#include "reader/glyf.h"
#include "reader/cmap.h"
#include "reader/head.h"
#include "reader/hhea.h"
#include "reader/loca.h"
#include "reader/kern.h"
#include "reader/name.h"
#include "reader/otff.h"
#include "reader/maxp.h"
#include "reader/hmtx.h"

typedef struct {
    int x, y;
    int width;
    int height;

    float min_x, min_y;
    float max_x, max_y;
    int is_empty;
} font_size_t;

/*!
 * \brief The font_tables_t struct
 *
 * This is collector table struct
 */
typedef struct
{
    offset_table_t offsets;
    table_record_t *list_table_record;  // list size is offsets.num_tables
    character_table_t corr_character_table;
    character_to_glyph_index_mapping_table_t character_to_glyph_index_table;
    naming_table_t name_table;
    maximum_profile_t max_profile;
    index_to_loc_table_t *list_index_loc_to_tables; // list size is max_profile.glyphs_count
    horizontal_metrics_table_t hor_metrics_table;
    horizontal_header_table_t hor_header_table;
    font_header_table_t header_table;
    glyph_t *list_glyph; // list of glyphs (count is max_profile.glyphs_count)

    // own temporary
    font_size_t *list_font_sizes;
    int16_t list_font_sizes_count;
} font_tables_t;

#endif // FONT_TABLES_H
