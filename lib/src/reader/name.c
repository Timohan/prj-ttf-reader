/*!
 * \file
 * \brief file name.c
 *
 * Naming table
 * https://docs.microsoft.com/en-us/typography/opentype/spec/name
 *
 * Copyright of Timo Hannukkala. All rights reserved.
 *
 * \author Timo Hannukkala <timo.hannukkala@hotmail.com>
 */

#include "name.h"
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "parse_value.h"

/*!
 * \brief name_parse_name_record
 *
 * Parse name_record
 * https://docs.microsoft.com/en-us/typography/opentype/spec/name
 *
 * \param data
 * \param data_size
 * \param offset
 * \param record [out]
 * \return 0 on success
 */
static int name_parse_name_record(const uint8_t* data, size_t data_size, size_t *offset, name_record_t *record)
{
    int ret;
    ret = parse_value_16u(data, data_size, offset, &record->platform_id);
    if (ret) {
        return ret;
    }
    ret = parse_value_16u(data, data_size, offset, &record->encoding_id);
    if (ret) {
        return ret;
    }
    ret = parse_value_16u(data, data_size, offset, &record->language_id);
    if (ret) {
        return ret;
    }
    ret = parse_value_16u(data, data_size, offset, &record->m_nameID);
    if (ret) {
        return ret;
    }
    ret = parse_value_16u(data, data_size, offset, &record->length);
    if (ret) {
        return ret;
    }
    ret = parse_value_16u(data, data_size, offset, &record->offset);
    if (ret) {
        return ret;
    }
    return 0;
}

/*!
 * \brief name_parse_naming_table
 *
 * Parse naming table
 * https://docs.microsoft.com/en-us/typography/opentype/spec/name
 *
 * \param data
 * \param data_size
 * \param offset
 * \param table
 * \return 0 on success
 */
int name_parse_naming_table(const uint8_t* data, size_t data_size, size_t offset, naming_table_t *table)
{
    int ret;
    uint16_t i;
    size_t offset_start = offset;
    ret = parse_value_16u(data, data_size, &offset, &table->format);
    if (ret) {
        return ret;
    }
    ret = parse_value_16u(data, data_size, &offset, &table->count);
    if (ret) {
        return ret;
    }
    ret = parse_value_16u(data, data_size, &offset, &table->string_offset);
    if (ret) {
        return ret;
    }

    if (table->count) {
        table->list_name_record = (name_record_t *)malloc(sizeof(name_record_t)*table->count);
        if (!table->list_name_record) {
            return errno;
        }
        memset(table->list_name_record, 0, sizeof(name_record_t)*table->count);
    }

    for (i=0;i<table->count;i++) {
        ret = name_parse_name_record(data, data_size, &offset, &table->list_name_record[i]);
        if (ret) {
            return ret;
        }

        if (table->list_name_record[i].length
                && data[offset_start + table->string_offset + table->list_name_record[i].offset] != '\0') {
            table->list_name_record[i].string = (char *)malloc((uint32_t)(table->list_name_record[i].length+1));
            if (!table->list_name_record[i].string) {
                return errno;
            }
            memcpy(table->list_name_record[i].string,
                   data + offset_start + table->string_offset + table->list_name_record[i].offset,
                   table->list_name_record[i].length);
            table->list_name_record[i].string[table->list_name_record[i].length] = '\0';
        }
    }

    return 0;
}

/*!
 * \brief name_clear
 *
 * Clears the naming table
 *
 * \param table naming table to clear
 */
void name_clear(naming_table_t *table)
{
    uint16_t i;
    if (table->list_name_record) {
        for (i=0;i<table->count;i++) {
            if (table->list_name_record[i].string) {
                free(table->list_name_record[i].string);
            }
        }
        free(table->list_name_record);
    }
    if (table->list_language_tag_record) {
        free(table->list_language_tag_record);
    }
}
