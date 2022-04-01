/*!
 * \file
 * \brief file cmap.h
 *
 * Character to glyph Index Mapping Table
 * https://docs.microsoft.com/en-us/typography/opentype/spec/cmap
 *
 * Copyright of Timo Hannukkala. All rights reserved.
 *
 * \author Timo Hannukkala <timohannukkala@hotmail.com>
 */

#ifndef CMAP_H
#define CMAP_H

#include <stdint.h>
#include <stddef.h>

/*!
 * \brief The character_table_t struct
 *
 * This table is to help to get character by glyphIndex
 */
typedef struct
{
    uint32_t character[UINT16_MAX];
} character_table_t;

/*!
 * \brief The encoding_record_format4_t struct
 *
 * Format 4: Segment mapping to delta values
 * This is the standard character-to-glyph-index mapping table for the Windows platform
 * for fonts that support Unicode BMP characters.
 *
 * https://docs.microsoft.com/en-us/typography/opentype/spec/cmap
 */
typedef struct {
    uint16_t format;
    uint16_t length;
    uint16_t language;
    uint16_t seg_count_x2;
    uint16_t search_range;
    uint16_t entry_selector;
    uint16_t range_shift;
    uint16_t *end_code;
    uint16_t reserved_pad;
    uint16_t *start_code;
    uint16_t *id_delta;
    uint16_t *id_range_offset;
    uint16_t glyph_id_array[UINT16_MAX+1];
} encoding_record_format4_t;

/*!
 * \brief The encoding_record_format6_t struct
 *
 * Format 6: Trimmed table mapping
 *
 * https://docs.microsoft.com/en-us/typography/opentype/spec/cmap
 */
typedef struct {
    uint16_t format;
    uint16_t length;
    uint16_t language;
    uint16_t first_code;
    uint16_t entry_count;
    uint16_t *glyph_id_array;
} encoding_record_format6_t;

/*!
 * \brief The sequential_map_group_t struct
 *
 * Format 12: Segmented coverage's SequentialMapGroup
 *
 * https://docs.microsoft.com/en-us/typography/opentype/spec/cmap
 */
typedef struct {
    uint32_t start_char_code;
    uint32_t end_char_code;
    uint32_t start_glyph_id;
} sequential_map_group_t;

/*!
 * \brief The encoding_record_format12_t struct
 *
 * Format 12: Segmented coverage
 *
 * https://docs.microsoft.com/en-us/typography/opentype/spec/cmap
 */
typedef struct {
    uint16_t format;
    uint16_t reserved;
    uint32_t length;
    uint32_t language;
    uint32_t numGroups;
    sequential_map_group_t *groups;
} encoding_record_format12_t;

/*!
 * \brief The encoding_record_t struct
 * https://docs.microsoft.com/en-us/typography/opentype/spec/cmap
 */
typedef struct {
    uint16_t platform_id;
    uint16_t encoding_id;
    uint32_t offset;

    encoding_record_format4_t *format4;     // Format 4
    encoding_record_format6_t *format6;     // Format 6
    encoding_record_format12_t *format12;   // Format 12
} encoding_record_t;

/*!
 * \brief The character_to_glyph_index_mapping_table_t struct
 * https://docs.microsoft.com/en-us/typography/opentype/spec/cmap
 */
typedef struct {
    uint16_t version;
    uint16_t num_tables;
    encoding_record_t *list_encoding_record;   // size is num_tables
} character_to_glyph_index_mapping_table_t;

int cmap_parse_character_to_glyph_index_mapping_table(const uint8_t* data, size_t data_size,
                                                      size_t offset,
                                                      character_to_glyph_index_mapping_table_t *character_to_glyph_index_table,
                                                      character_table_t *corr_character_table);
void cmap_clear(character_to_glyph_index_mapping_table_t *character_to_glyph_index_table);

#endif // CMAP_H
