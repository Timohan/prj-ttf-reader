/*!
 * \file
 * \brief file loca.h
 *
 * index to Location
 * https://docs.microsoft.com/en-us/typography/opentype/spec/loca
 *
 * Copyright of Timo Hannukkala. All rights reserved.
 *
 * \author Timo Hannukkala <timohannukkala@hotmail.com>
 */

#ifndef LOCA_H
#define LOCA_H

#include <stdint.h>
#include <stddef.h>
/*!
 * \brief The index_to_loc_table_t struct
 *
 * IndexToLocTable
 * https://docs.microsoft.com/en-us/typography/opentype/spec/loca
 *
 * "If a glyph has no outline, then loca[n] = loca [n+1]".
 */
typedef struct {
    uint32_t offset;
} index_to_loc_table_t;

int loca_parse(const uint8_t* data, size_t data_size, size_t offset, index_to_loc_table_t **list_tables, uint16_t table_count, int16_t format);
void loca_clear(index_to_loc_table_t **list_tables);

#endif // LOCA_H
