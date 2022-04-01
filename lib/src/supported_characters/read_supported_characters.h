/*!
 * \file
 * \brief file read_supported_characters.h
 *
 * Read supported characters from ttf
 *
 * Copyright of Timo Hannukkala. All rights reserved.
 *
 * \author Timo Hannukkala <timohannukkala@hotmail.com>
 */

#ifndef READ_SUPPORTED_CHARACTERS_H
#define READ_SUPPORTED_CHARACTERS_H

#include <stdint.h>
#include <stddef.h>
#include "../font_tables.h"

int prj_ttf_reader_parse_supported_characters(const uint8_t *data, size_t data_size, font_tables_t *tables, prj_ttf_reader_supported_characters_t *supported_characters);

#endif // READ_SUPPORTED_CHARACTERS_H
