/*!
 * \file
 * \brief file otff.h
 * https://docs.microsoft.com/en-us/typography/opentype/spec/otff
 * Main tables of the tff
 *
 * Copyright of Timo Hannukkala. All rights reserved.
 *
 * \author Timo Hannukkala <timohannukkala@hotmail.com>
 */

#ifndef OTFF_H
#define OTFF_H

#include <stddef.h>
#include <stdint.h>

/*!
 * \brief The offset_table_t struct
 *
 * https://docs.microsoft.com/en-us/typography/opentype/spec/otff
 *
 * Offset Table
 */
typedef struct
{
    uint32_t sfnt_version;
    uint16_t num_tables;
    uint16_t search_range;
    uint16_t entry_selector;
    uint16_t range_shift;
} offset_table_t;

/*!
 * All table tag strings such "kern" or "glyf" are 4 bytes
 */
#define TABLE_TAG_STRING_SIZE   4

/*!
 * \brief The table_record_t struct
 *
 * https://docs.microsoft.com/en-us/typography/opentype/spec/otff
 *
 * Table record
 *
 */
typedef struct
{
    uint32_t table_tag;
    char table_tag_string[TABLE_TAG_STRING_SIZE+1];
    uint32_t check_sum;
    uint32_t offset;
    uint32_t length;
} table_record_t;

uint16_t otff_get_table_record_index(table_record_t *list_table_record, uint16_t list_table_record_count, const char *tag);
int otff_parse_table_records(const uint8_t* data, size_t data_size, size_t *offset, table_record_t **list_records, const offset_table_t *offsets);
int otff_parse_offset_table(const uint8_t* data, size_t data_size, size_t *offset, offset_table_t *offsets);
void otff_clear(table_record_t **list_table_record);

#endif // OTFF_H
