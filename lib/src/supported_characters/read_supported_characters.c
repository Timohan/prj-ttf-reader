/*!
 * \file
 * \brief file read_supported_characters.c
 *
 * Read supported characters from ttf
 *
 * Copyright of Timo Hannukkala. All rights reserved.
 *
 * \author Timo Hannukkala <timohannukkala@hotmail.com>
 */

#include "read_supported_characters.h"
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "../prj-ttf-reader.h"

/*!
 * \brief prj_ttf_reader_parse_data
 *
 * parse the ttf data and set supported_characters
 *
 * \param data
 * \param data_size
 * \param tables
 * \param supported_characters [out] supported characters from ttf will
 * be filled here
 * \return 0 on success
 */
int prj_ttf_reader_parse_supported_characters(const uint8_t *data, size_t data_size,
                                              font_tables_t *tables,
                                              prj_ttf_reader_supported_characters_t *supported_characters)
{
    int ret;
    uint32_t i;
    size_t offset = 0;

    ret = otff_parse_offset_table(data, data_size, &offset, &tables->offsets);
    if (ret) {
        return ret;
    }

    ret = otff_parse_table_records(data, data_size, &offset, &tables->list_table_record, &tables->offsets);
    if (ret) {
        return ret;
    }

    i = otff_get_table_record_index(tables->list_table_record, tables->offsets.num_tables, "maxp");
    if (UINT16_MAX == i) {
        return EINVAL;
    }

    ret = maxp_parse_maximum_profile(data, data_size, tables->list_table_record[i].offset, &tables->max_profile);
    if (ret) {
        return ret;
    }

    i = otff_get_table_record_index(tables->list_table_record, tables->offsets.num_tables, "cmap");
    if (UINT16_MAX == i) {
        return EINVAL;
    }

    ret = cmap_parse_character_to_glyph_index_mapping_table(data, data_size, tables->list_table_record[i].offset,
                                                            &tables->character_to_glyph_index_table, &tables->corr_character_table);
    if (ret) {
        return ret;
    }

    i = otff_get_table_record_index(tables->list_table_record, tables->offsets.num_tables, "hmtx");
    if (UINT16_MAX == i) {
        return EINVAL;
    }


    ret = hmtx_parse(data, data_size,
                     &tables->hor_metrics_table, tables->list_table_record[i].offset,
                     &tables->hor_header_table, &tables->max_profile);
    if (ret) {
        return ret;
    }

    if (tables->max_profile.glyphs_count) {
        supported_characters->character_list_count = 0;
        supported_characters->list_character = (uint32_t *)malloc(sizeof(uint32_t)*tables->max_profile.glyphs_count);
        for (i=0;i<tables->max_profile.glyphs_count;i++) {
            if (tables->corr_character_table.character[i]) {
                supported_characters->list_character[supported_characters->character_list_count] = tables->corr_character_table.character[i];
                supported_characters->character_list_count++;
            }
        }

        if (!supported_characters->character_list_count) {
            free(supported_characters->list_character);
            supported_characters->list_character = NULL;
        } else if (supported_characters->character_list_count != tables->max_profile.glyphs_count) {
            supported_characters->list_character = realloc(supported_characters->list_character, sizeof(uint32_t)*supported_characters->character_list_count);
        }
    }

    return 0;
}