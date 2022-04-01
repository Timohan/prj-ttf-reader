/*!
 * \file
 * \brief file loca.c
 *
 * index to Location
 * https://docs.microsoft.com/en-us/typography/opentype/spec/loca
 *
 * Copyright of Timo Hannukkala. All rights reserved.
 *
 * \author Timo Hannukkala <timohannukkala@hotmail.com>
 */

#include "loca.h"
#include <stdlib.h>
#include <errno.h>
#include "parse_value.h"

/*!
 * \brief loca_parse
 *
 * https://docs.microsoft.com/en-us/typography/opentype/spec/loca
 *
 * allocs list_tables and parses the loca
 *
 * \param data
 * \param data_size
 * \param offset
 * \param list_tables list of tables
 * \param table_count count of tables (max_profile.glyphs_count+1)
 * \param format
 * \return 0 on success
 */
int loca_parse(const uint8_t* data, size_t data_size, size_t offset, index_to_loc_table_t **list_tables, uint16_t table_count, int16_t format)
{
    int ret;
    uint16_t i;
    uint16_t val;
    *list_tables = (index_to_loc_table_t *)malloc(sizeof(index_to_loc_table_t)*table_count);
    if (!*list_tables ) {
        return errno;
    }

    if (format == 0) {
        // short version
        for (i=0;i<table_count;i++) {
            ret = parse_value_16u(data, data_size, &offset, &val);
            if (ret) {
                return ret;
            }
            (*list_tables)[i].offset = (uint32_t)(val)*2;
        }
    } else {
        // long version
        for (i=0;i<table_count;i++) {
            ret = parse_value_32u(data, data_size, &offset, &(*list_tables)[i].offset);
            if (ret) {
                return ret;
            }
        }
    }
    return 0;
}

/*!
 * \brief loca_clear
 * \param list_tables
 */
void loca_clear(index_to_loc_table_t **list_tables)
{
    if (*list_tables) {
        free(*list_tables);
        *list_tables = NULL;
    }
}
