/*!
 * \file
 * \brief file cmap.c
 *
 * Character to glyph Index Mapping Table
 * https://docs.microsoft.com/en-us/typography/opentype/spec/cmap
 *
 * Copyright of Timo Hannukkala. All rights reserved.
 *
 * \author Timo Hannukkala <timo.hannukkala@hotmail.com>
 */

#include "cmap.h"
#include "parse_value.h"
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

/*!
 * \brief cmap_parse_character_to_glyph_index_mapping_table_subtable4
 *
 * https://docs.microsoft.com/en-us/typography/opentype/spec/cmap
 * parse format4 subtable
 *
 * \param data
 * \param data_size
 * \param offset
 * \param format4
 * \param corr_character_table
 * \return 0 on success
 */
static int cmap_parse_character_to_glyph_index_mapping_table_subtable4(const uint8_t* data, size_t data_size, size_t offset,
                                                         encoding_record_format4_t *format4,
                                                         character_table_t *corr_character_table)
{
    int ret;
    uint16_t i;
    uint32_t f;
    size_t range_start_offset;
    size_t range_offset;

    ret = parse_value_16u(data, data_size, &offset, &format4->length);
    if (ret) {
        return ret;
    }
    ret = parse_value_16u(data, data_size, &offset, &format4->language);
    if (ret) {
        return ret;
    }
    ret = parse_value_16u(data, data_size, &offset, &format4->seg_count_x2);
    if (ret) {
        return ret;
    }
    ret = parse_value_16u(data, data_size, &offset, &format4->search_range);
    if (ret) {
        return ret;
    }
    ret = parse_value_16u(data, data_size, &offset, &format4->entry_selector);
    if (ret) {
        return ret;
    }

    ret = parse_value_16u(data, data_size, &offset, &format4->range_shift);
    if (ret) {
        return ret;
    }

    if (format4->seg_count_x2) {
        format4->end_code = (uint16_t *)malloc(sizeof(uint16_t)*format4->seg_count_x2/2);
        if (!format4->end_code) {
            return errno;
        }
        for (i=0;i<format4->seg_count_x2/2;i++) {
            ret = parse_value_16u(data, data_size, &offset, &format4->end_code[i]);
            if (ret) {
                return ret;
            }
        }
    }

    ret = parse_value_16u(data, data_size, &offset, &format4->reserved_pad);
    if (ret) {
        return ret;
    }

    if (format4->seg_count_x2) {
        format4->start_code = (uint16_t *)malloc(sizeof(uint16_t)*format4->seg_count_x2/2);
        if (!format4->start_code) {
            return errno;
        }
        for (i=0;i<format4->seg_count_x2/2;i++) {
            ret = parse_value_16u(data, data_size, &offset, &format4->start_code[i]);
            if (ret) {
                return ret;
            }
        }
        format4->id_delta = (uint16_t *)malloc(sizeof(uint16_t)*format4->seg_count_x2/2);
        if (!format4->id_delta) {
            return errno;
        }
        for (i=0;i<format4->seg_count_x2/2;i++) {
            ret = parse_value_16u(data, data_size, &offset, &format4->id_delta[i]);
            if (ret) {
                return ret;
            }
        }

        range_start_offset = offset;
        format4->id_range_offset = (uint16_t *)malloc(sizeof(uint16_t)*format4->seg_count_x2/2);
        if (!format4->id_range_offset) {
            return errno;
        }
        for (i=0;i<format4->seg_count_x2/2;i++) {
            ret = parse_value_16u(data, data_size, &offset, &format4->id_range_offset[i]);
            if (ret) {
                return ret;
            }
        }

        for (i=0;i<format4->seg_count_x2/2;i++) {
            if (!format4->id_range_offset[i]) {
                for (f=format4->start_code[i];f<=format4->end_code[i];f++) {
                    if (f <= UINT16_MAX) {
                        format4->glyph_id_array[f] = (uint16_t)(f + format4->id_delta[i]);
                        corr_character_table->character[ format4->glyph_id_array[f] ] = f;
                    }
                }
            } else {
                range_offset = range_start_offset + format4->id_range_offset[i] + sizeof(uint16_t)*i;
                for (f=format4->start_code[i];f<=format4->end_code[i];f++) {
                    ret = parse_value_16u(data, data_size, &range_offset, &format4->glyph_id_array[f]);
                    if (ret) {
                        return ret;
                    }
                    corr_character_table->character[ format4->glyph_id_array[f] ] = f;
                }
            }
        }
    }

    return 0;
}

/*!
 * \brief cmap_parse_character_to_glyph_index_mapping_table_subtable6
 *
 * https://docs.microsoft.com/en-us/typography/opentype/spec/cmap
 * parse format6 subtable
 *
 * \param data
 * \param data_size
 * \param offset
 * \param format6
 * \param corr_character_table
 * \return 0 on success
 */
static int cmap_parse_character_to_glyph_index_mapping_table_subtable6(const uint8_t* data, size_t data_size, size_t offset,
                                                         encoding_record_format6_t *format6,
                                                         character_table_t *corr_character_table)
{
    uint16_t i;
    int ret;
    ret = parse_value_16u(data, data_size, &offset, &format6->length);
    if (ret) {
        return ret;
    }
    if (!format6->length) {
        return EIO;
    }
    ret = parse_value_16u(data, data_size, &offset, &format6->language);
    if (ret) {
        return ret;
    }
    ret = parse_value_16u(data, data_size, &offset, &format6->first_code);
    if (ret) {
        return ret;
    }
    ret = parse_value_16u(data, data_size, &offset, &format6->entry_count);
    if (ret) {
        return ret;
    }

    if (!format6->entry_count) {
        return EIO;
    }

    format6->entry_count /= (uint16_t)sizeof(uint16_t);
    format6->glyph_id_array = (uint16_t *)calloc(format6->entry_count, sizeof(uint16_t));
    if (!format6->glyph_id_array) {
        return errno;
    }

    for (i=0;i<format6->entry_count;i++) {
        ret = parse_value_16u(data, data_size, &offset, &format6->glyph_id_array[i]);
        if (ret) {
            return ret;
        }

        corr_character_table->character[ format6->glyph_id_array[i] ] = (uint32_t)(i+format6->first_code);
    }

    return 0;
}

/*!
 * \brief cmap_parse_character_to_glyph_index_mapping_table_subtable12
 *
 * https://docs.microsoft.com/en-us/typography/opentype/spec/cmap
 * parse format12 subtable
 *
 * \param data
 * \param data_size
 * \param offset
 * \param format12
 * \param corr_character_table
 * \return 0 on success
 */
static int cmap_parse_character_to_glyph_index_mapping_table_subtable12(const uint8_t* data, size_t data_size, size_t offset,
                                                         encoding_record_format12_t *format12,
                                                         character_table_t *corr_character_table)
{
    uint32_t i;
    uint32_t s;
    int ret;
    ret = parse_value_16u(data, data_size, &offset, &format12->reserved);
    if (ret) {
        return ret;
    }

    ret = parse_value_32u(data, data_size, &offset, &format12->length);
    if (ret) {
        return ret;
    }

    if (!format12->length) {
        return EIO;
    }

    ret = parse_value_32u(data, data_size, &offset, &format12->language);
    if (ret) {
        return ret;
    }

    ret = parse_value_32u(data, data_size, &offset, &format12->numGroups);
    if (ret) {
        return ret;
    }

    format12->groups = (sequential_map_group_t *)malloc(format12->numGroups*sizeof(sequential_map_group_t));
    if (!format12->groups) {
        return errno;
    }

    for (i=0;i<format12->numGroups;i++) {
        ret = parse_value_32u(data, data_size, &offset, &format12->groups[i].start_char_code);
        if (ret) {
            return ret;
        }
        ret = parse_value_32u(data, data_size, &offset, &format12->groups[i].end_char_code);
        if (ret) {
            return ret;
        }
        ret = parse_value_32u(data, data_size, &offset, &format12->groups[i].start_glyph_id);
        if (ret) {
            return ret;
        }

        for (s=0;s<=format12->groups[i].end_char_code-format12->groups[i].start_char_code;s++) {
            if (format12->groups[i].start_glyph_id+s < UINT16_MAX
                    && format12->groups[i].start_glyph_id+s < UINT16_MAX) {
                corr_character_table->character[format12->groups[i].start_glyph_id+s] = format12->groups[i].start_char_code+s;
            }
        }
    }

    return 0;
}

/*!
 * \brief cmap_parse_character_to_glyph_index_mapping_table
 *
 * https://docs.microsoft.com/en-us/typography/opentype/spec/cmap
 *
 * \param data
 * \param data_size
 * \param offset
 * \param character_to_glyph_index_table
 * \param corr_character_table
 * \return 0 on success
 */
int cmap_parse_character_to_glyph_index_mapping_table(const uint8_t* data, size_t data_size, size_t offset,
                                                      character_to_glyph_index_mapping_table_t *character_to_glyph_index_table,
                                                      character_table_t *corr_character_table)
{
    int ret;
    uint16_t i;
    size_t start_offset = offset;
    size_t tmp_offset;
    uint16_t format;
    ret = parse_value_16u(data, data_size, &offset, &character_to_glyph_index_table->version);
    if (ret) {
        return ret;
    }
    ret = parse_value_16u(data, data_size, &offset, &character_to_glyph_index_table->num_tables);
    if (ret) {
        return ret;
    }

    if (!character_to_glyph_index_table->num_tables) {
        return 0;
    }

    character_to_glyph_index_table->list_encoding_record = (encoding_record_t *)malloc(sizeof(encoding_record_t)*character_to_glyph_index_table->num_tables);
    if (!character_to_glyph_index_table->list_encoding_record) {
        return errno;
    }
    memset(character_to_glyph_index_table->list_encoding_record, 0, sizeof(encoding_record_t)*character_to_glyph_index_table->num_tables);
    for (i=0;i<character_to_glyph_index_table->num_tables;i++) {
        ret = parse_value_16u(data, data_size, &offset, &character_to_glyph_index_table->list_encoding_record[i].platform_id);
        if (ret) {
            return ret;
        }
        ret = parse_value_16u(data, data_size, &offset, &character_to_glyph_index_table->list_encoding_record[i].encoding_id);
        if (ret) {
            return ret;
        }
        ret = parse_value_32u(data, data_size, &offset, &character_to_glyph_index_table->list_encoding_record[i].offset);
        if (ret) {
            return ret;
        }

        tmp_offset = start_offset + character_to_glyph_index_table->list_encoding_record[i].offset;
        ret = parse_value_16u(data, data_size, &tmp_offset, &format);
        if (ret) {
            return ret;
        }

        switch (format) {
            case 4:
                // format 4 is pretty good, so if it's format 4
                // it's enough for cmap
                character_to_glyph_index_table->list_encoding_record[i].format4 = (encoding_record_format4_t *)calloc(1, sizeof(encoding_record_format4_t));
                if (!character_to_glyph_index_table->list_encoding_record[i].format4) {
                    return errno;
                }
                character_to_glyph_index_table->list_encoding_record[i].format4->format = format;
                return cmap_parse_character_to_glyph_index_mapping_table_subtable4(data, data_size, tmp_offset,
                                                                                   character_to_glyph_index_table->list_encoding_record[i].format4,
                                                                                   corr_character_table);
            case 6:
                character_to_glyph_index_table->list_encoding_record[i].format6 = (encoding_record_format6_t *)calloc(1, sizeof(encoding_record_format6_t));
                if (!character_to_glyph_index_table->list_encoding_record[i].format6) {
                    return errno;
                }
                character_to_glyph_index_table->list_encoding_record[i].format6->format = format;
                ret = cmap_parse_character_to_glyph_index_mapping_table_subtable6(data, data_size, tmp_offset,
                                                                                  character_to_glyph_index_table->list_encoding_record[i].format6,
                                                                                  corr_character_table);
                if (ret) {
                    return ret;
                }
                break;
            case 12:
                character_to_glyph_index_table->list_encoding_record[i].format12 = (encoding_record_format12_t *)calloc(1, sizeof(encoding_record_format12_t));
                character_to_glyph_index_table->list_encoding_record[i].format12->format = format;
                ret = cmap_parse_character_to_glyph_index_mapping_table_subtable12(data, data_size, tmp_offset,
                                                                                  character_to_glyph_index_table->list_encoding_record[i].format12,
                                                                                  corr_character_table);
                if (ret) {
                    return ret;
                }
                break;
            default:
                break;
        }
    }

    return 0;
}

/*!
 * \brief cmap_clear
 *
 * Clears the all values in character_to_glyph_index_table
 *
 * \param character_to_glyph_index_table
 */
void cmap_clear(character_to_glyph_index_mapping_table_t *character_to_glyph_index_table)
{
    uint16_t i;
    if (character_to_glyph_index_table->list_encoding_record) {
        for (i=0;i<character_to_glyph_index_table->num_tables;i++) {
            if (character_to_glyph_index_table->list_encoding_record[i].format4) {
                if (character_to_glyph_index_table->list_encoding_record[i].format4->end_code) {
                    free(character_to_glyph_index_table->list_encoding_record[i].format4->end_code);
                }
                if (character_to_glyph_index_table->list_encoding_record[i].format4->id_delta) {
                    free(character_to_glyph_index_table->list_encoding_record[i].format4->id_delta);
                }
                if (character_to_glyph_index_table->list_encoding_record[i].format4->start_code) {
                    free(character_to_glyph_index_table->list_encoding_record[i].format4->start_code);
                }
                if (character_to_glyph_index_table->list_encoding_record[i].format4->id_range_offset) {
                    free(character_to_glyph_index_table->list_encoding_record[i].format4->id_range_offset);
                }
                free(character_to_glyph_index_table->list_encoding_record[i].format4);
            }
            if (character_to_glyph_index_table->list_encoding_record[i].format6) {
                if (character_to_glyph_index_table->list_encoding_record[i].format6->glyph_id_array) {
                    free(character_to_glyph_index_table->list_encoding_record[i].format6->glyph_id_array);
                }
                free(character_to_glyph_index_table->list_encoding_record[i].format6);
            }
            if (character_to_glyph_index_table->list_encoding_record[i].format12) {
                if (character_to_glyph_index_table->list_encoding_record[i].format12->groups) {
                    free(character_to_glyph_index_table->list_encoding_record[i].format12->groups);
                }
                free(character_to_glyph_index_table->list_encoding_record[i].format12);
            }
        }
        free(character_to_glyph_index_table->list_encoding_record);
    }
}
