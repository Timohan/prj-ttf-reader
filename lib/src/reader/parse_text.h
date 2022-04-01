/*!
 * \file
 * \brief file parse_text.h
 *
 * Parsing utf8 text string (uint8) to list of chars (uint32)
 *
 * Copyright of Timo Hannukkala. All rights reserved.
 *
 * \author Timo Hannukkala <timohannukkala@hotmail.com>
 */

#ifndef PARSE_FONT_H
#define PARSE_FONT_H

#include <stdint.h>

uint32_t *parse_text_generate_list_characters(const char *text, uint32_t *list_characters_size, int allow_dublicate_characters);

#endif // PARSE_FONT_H
