/*!
 * \file
 * \brief file name.h
 *
 * Naming table
 * https://docs.microsoft.com/en-us/typography/opentype/spec/name
 *
 * Copyright of Timo Hannukkala. All rights reserved.
 *
 * \author Timo Hannukkala <timo.hannukkala@hotmail.com>
 */

#ifndef NAME_H
#define NAME_H

#include <stdint.h>
#include <stddef.h>

/*!
 * \brief The name_record_t struct
 * https://docs.microsoft.com/en-us/typography/opentype/spec/name
 */
typedef struct {
    uint16_t platform_id;
    uint16_t encoding_id;
    uint16_t language_id;
    uint16_t m_nameID;
    uint16_t length;
    uint16_t offset;

    char *string;
} name_record_t;

/*!
 * \brief The lang_tag_record_t struct
 * https://docs.microsoft.com/en-us/typography/opentype/spec/name
 */
typedef struct {
    uint16_t length;
    uint16_t offset;
} lang_tag_record_t;

/*!
 * \brief The naming_table_t struct
 * https://docs.microsoft.com/en-us/typography/opentype/spec/name
 */
typedef struct {
    uint16_t format;
    uint16_t count;
    uint16_t string_offset;
    name_record_t *list_name_record;
    uint16_t list_language_tag_record_count;
    lang_tag_record_t *list_language_tag_record;
} naming_table_t;

int name_parse_naming_table(const uint8_t* data, size_t data_size, size_t offset, naming_table_t *table);
void name_clear(naming_table_t *table);

#endif // NAME_H
