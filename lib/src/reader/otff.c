/*!
 * \file
 * \brief file otff.c
 *
 * Main tables of the tff
 *
 * Copyright of Timo Hannukkala. All rights reserved.
 *
 * \author Timo Hannukkala <timo.hannukkala@hotmail.com>
 */

#include "otff.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "parse_value.h"

/*!
 * \brief otff_get_table_record_index
 *
 * get index of tag string in list_table_record
 *
 * \param list_table_record
 * \param list_table_record_count
 * \param tag
 * \return UINT16_MAX if tag not found, index otherwise
 */
uint16_t otff_get_table_record_index(table_record_t *list_table_record, uint16_t list_table_record_count, const char *tag)
{
    uint16_t i;
    for (i=0;i<list_table_record_count;i++) {
        if (!strcmp(list_table_record[i].table_tag_string, tag)) {
            return i;
        }
    }

    return UINT16_MAX;
}

/*!
 * \brief otff_parse_table_single_record
 *
 * Parse single table record
 * https://docs.microsoft.com/en-us/typography/opentype/spec/otff
 *
 * \param data
 * \param data_size
 * \param offset
 * \param record
 * \return
 */
static int otff_parse_table_single_record(const uint8_t* data, size_t data_size, size_t *offset, table_record_t *record)
{
    int ret;
    ret = parse_value_32u(data, data_size, offset, &record->table_tag);
    if (ret) {
        return ret;
    }
    memcpy(&record->table_tag_string, data+((*offset)-TABLE_TAG_STRING_SIZE), TABLE_TAG_STRING_SIZE);
    record->table_tag_string[TABLE_TAG_STRING_SIZE] = '\0';

    ret = parse_value_32u(data, data_size, offset, &record->check_sum);
    if (ret) {
        return ret;
    }
    ret = parse_value_32u(data, data_size, offset, &record->offset);
    if (ret) {
        return ret;
    }
    ret = parse_value_32u(data, data_size, offset, &record->length);
    if (ret) {
        return ret;
    }
    return 0;
}


/*!
 * \brief otff_parse_table_records
 *
 * Fills the table records
 * https://docs.microsoft.com/en-us/typography/opentype/spec/otff
 *
 * \param data
 * \param data_size
 * \param offset [in/out]
 * \param list_records [out]
 * \param offsets
 * \return
 */
int otff_parse_table_records(const uint8_t* data, size_t data_size, size_t *offset, table_record_t **list_records, const offset_table_t *offsets)
{
    int ret;
    uint16_t i;
    table_record_t *record;
    *list_records = (table_record_t *)malloc(sizeof(table_record_t)*offsets->num_tables);
    if (!*list_records) {
        return errno;
    }
    record = *list_records;
    for (i=0;i<offsets->num_tables;i++) {
        ret = otff_parse_table_single_record(data, data_size, offset, &record[i]);
        if (ret) {
            return ret;
        }
    }

    return 0;
}

/*!
 * \brief otff_parse_offset_table
 *
 * Parse the header
 * From: https://docs.microsoft.com/en-us/typography/opentype/spec/otff
 *
 * \param data
 * \param data_size
 * \param offset [in/out]
 * \param offsets [out]
 * \return 0 on success
 */
int otff_parse_offset_table(const uint8_t* data, size_t data_size, size_t *offset, offset_table_t *offsets)
{
    int ret;
    ret = parse_value_32u(data, data_size, offset, &offsets->sfnt_version);
    if (ret) {
        return ret;
    }
    ret = parse_value_16u(data, data_size, offset, &offsets->num_tables);
    if (ret || !offsets->num_tables) {
        return ret;
    }
    ret = parse_value_16u(data, data_size, offset, &offsets->search_range);
    if (ret) {
        return ret;
    }
    ret = parse_value_16u(data, data_size, offset, &offsets->entry_selector);
    if (ret) {
        return ret;
    }
    ret = parse_value_16u(data, data_size, offset, &offsets->range_shift);
    return ret;
}

/*!
 * \brief otff_clear
 * \param list_table_record
 */
void otff_clear(table_record_t **list_table_record)
{
    if (*list_table_record) {
        free(*list_table_record);
        *list_table_record = NULL;
    }
}
