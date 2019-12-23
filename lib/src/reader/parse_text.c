/*!
 * \file
 * \brief file parse_text.c
 *
 * Parsing utf8 text string (uint8) to list of chars (uint32)
 *
 * Copyright of Timo Hannukkala. All rights reserved.
 *
 * \author Timo Hannukkala <timo.hannukkala@hotmail.com>
 */

#include "parse_text.h"
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

/*!
 * \brief is_character_on_list
 * \param list_characters
 * \param list_characters_size
 * \param character
 * \return 0 if it's on the list
 */
static int is_character_on_list(const uint32_t *list_characters, uint32_t list_characters_size, uint32_t character)
{
    uint32_t i;
    for (i=0;i<list_characters_size;i++) {
        if (character == list_characters[i]) {
            return 0;
        }
    }

    return EINVAL;
}

/*!
 * \brief parse_text_convert_char_to_utf32
 *
 * Gets the first character of the text and converts it to utf32 character
 *
 * The character size can be 1, 2, 3 or 4
 * Reference for parsing
 * https://en.wikipedia.org/wiki/UTF-8
 *
 * \param text
 * \param character [out] first character
 * \param character_size [out] size of the first character, can be 1, 2, 3 or 4 bytes
 * \return 0 on success
 */
static int parse_text_convert_char_to_utf32(const char *text, uint32_t *character, uint32_t *character_size)
{
    if (!text) {
        return 1;
    }

    // hex 0x0 – 0x7F == 1 byte
    if (text[0] >= 0) {
        *character = (uint32_t)text[0];
        *character_size = 1;
        return 0;
    }

    uint32_t tmp;
    // 110xxxxx 10xxxxxx == 2 bytes
    // 0x0080 to 0x07FF
    if ((text[0] & 0xE0) == 0xC0 && (text[1] & 0xC0) == 0x80) {
        tmp = ((uint32_t)(text[0] & 0x1F) << 6) | (uint32_t)(text[1] & 0x3F);
        if (tmp >= 0x80) {
            *character = tmp;
            *character_size = 2;
            return 0;
        }
    }

    // 1110xxxx 10xxxxxx 10xxxxxx
    // 0x0800 to 0xFFFF
    if ((text[0] & 0xF0) == 0xE0 && (text[1] & 0xC0) == 0x80 && (text[2] & 0xC0) == 0x80) {
        tmp = ((uint32_t)(text[0] & 0x0F) << 12) | ((uint32_t)(text[1] & 0x3F) << 6) | (uint32_t)(text[2] & 0x3F);
        if (tmp >= 0x0800) {
            *character = tmp;
            *character_size = 3;
            return 0;
        }
    }

    // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
    // 0x10000 to 0x10FFFF
    if ((text[0] & 0xF8) == 0xF0 && (text[1] & 0xC0) == 0x80 && (text[2] & 0xC0) == 0x80 && (text[3] & 0xC0) == 0x80) {
        tmp = ((uint32_t)(text[0] & 0x07) << 18) | ((uint32_t)(text[1] & 0x3F) << 12) | ((uint32_t)(text[2] & 0x3F) << 6) | (uint32_t)(text[3] & 0x3F);
        if (tmp >= 0x10000) {
            *character = tmp;
            *character_size = 4;
            return 0;
        }
    }

    return EINVAL;
}

/*!
 * \brief parse_text_generate_list_characters
 *
 * parse utf8 text to char list (uint32 values)
 *
 * \param text this will be parsed to list of chars
 * \param list_characters_size the size of return value "list of chars"
 * \param allow_dublicate_characters if 1, then it allows duplicate characters to list, if 0 then
 * allows character index to be only once in the list
 * \return the list of chars
 */
uint32_t *parse_text_generate_list_characters(const char *text, uint32_t *list_characters_size, int allow_dublicate_characters)
{
    if (!text) {
        return NULL;
    }

    uint32_t *ret_tmp;
    uint32_t *ret = NULL;
    uint32_t ret_size = 0;
    uint32_t character;
    uint32_t character_size;

    while (*text) {
        if (parse_text_convert_char_to_utf32(text,
                                             &character,
                                             &character_size)) {
            break;
        }

        if (!ret) {
            ret = (uint32_t *)malloc(sizeof(uint32_t));
            if (!ret) {
                break;
            }
            ret[0] = character;
            ret_size = 1;

        } else {
            if (allow_dublicate_characters == 1 || is_character_on_list(ret, ret_size, character)) {
                ret_size++;
                ret_tmp = (uint32_t *)realloc(ret, sizeof(uint32_t)*ret_size);
                if (!ret_tmp) {
                    free(ret);
                    ret = NULL;
                    ret_size = 0;
                    break;
                }
                ret = ret_tmp;

                ret[ret_size-1] = character;
            }
        }

        text = text+character_size;
    }

    *list_characters_size = ret_size;
    return ret;
}

