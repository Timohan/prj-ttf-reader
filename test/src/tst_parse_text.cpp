/*!
 * \file
 * \brief file tst_parse_text.cpp
 *
 * test parse_text.cpp sources
 *
 * Copyright of Timo Hannukkala. All rights reserved.
 *
 * \author Timo Hannukkala <timohannukkala@hotmail.com>
 */

#include "tst_parse_text.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../lib/src/reader/parse_text.h"

/*!
 * \brief tst_parse_text_generate_list_characters
 *
 * tests parse_text_generate_list_characters
 *
 * \return 0 on success
 */
int tst_parse_text_generate_list_characters()
{
    char test_string[124];
    uint32_t i;
    uint32_t list_characters_size;
    uint32_t *ret = NULL;

    ret = parse_text_generate_list_characters("0123456789", &list_characters_size, 0);
    free(ret);
    if (list_characters_size != 10) {
        return 1;
    }

    ret = parse_text_generate_list_characters("0123456789öäåÖÄÅ漢字新字体𠜎𠜱𠝹𠱓", &list_characters_size, 0);
    free(ret);
    if (list_characters_size != 24) {
        return 2;
    }

    memset(test_string, 'a', 124);
    test_string[3] = '\0';
    test_string[1] = 'b';
    test_string[2] = 'c';

    ret = parse_text_generate_list_characters(test_string, &list_characters_size, 0);
    free(ret);
    if (list_characters_size != 3) {
        return 3;
    }

    test_string[1] = '\0';
    for (i=1;i<=127;i++) {
        test_string[0] = static_cast<char>(i);
        ret = parse_text_generate_list_characters(test_string, &list_characters_size, 0);
        if (list_characters_size != 1 || ret[0] != i) {
            free(ret);
            return 1000+static_cast<int>(i);
        }
        free(ret);
    }


    // 110xxxxx 10xxxxxx == 2 bytes
    // 0x0080 to 0x07FF
    test_string[4] = '\0';
    for (i=0x0080;i<=0x07FF;i++) {
        test_string[0] = static_cast<char>(0xC0 | static_cast<char>(i >> 6));
        test_string[1] = static_cast<char>(0x80 | static_cast<char>(i & 0x3F));
        test_string[2] = test_string[0];
        test_string[3] = test_string[1];

        ret = parse_text_generate_list_characters(test_string, &list_characters_size, 0);
        if (list_characters_size != 1 || ret[0] != i) {
            free(ret);
            return 1000+static_cast<int>(i);
        }
        free(ret);
    }

    // 1110xxxx 10xxxxxx 10xxxxxx
    // 0x0800 to 0xFFFF
    test_string[6] = '\0';
    for (i=0x0800;i<=0xFFFF;i++) {
        test_string[0] = static_cast<char>(0xE0 | static_cast<char>(i >> 12));
        test_string[1] = static_cast<char>(0x80 | static_cast<char>( (i >> 6) &  0x3F));
        test_string[2] = static_cast<char>(0x80 | static_cast<char>(i & 0x3F));
        test_string[3] = test_string[0];
        test_string[4] = test_string[1];
        test_string[5] = test_string[2];

        ret = parse_text_generate_list_characters(test_string, &list_characters_size, 0);
        if (list_characters_size != 1 || ret[0] != i) {
            free(ret);
            return 1000+static_cast<int>(i);
        }
        free(ret);
    }

    // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
    // 0x10000 to 0x10FFFF
    test_string[8] = '\0';
    for (i=0x10000;i<=0x10FFFF;i++) {
        test_string[0] = static_cast<char>(0xF0 | static_cast<char>(i >> 18));
        test_string[1] = static_cast<char>(0x80 | static_cast<char>( (i >> 12) &  0x3F));
        test_string[2] = static_cast<char>(0x80 | static_cast<char>( (i >> 6) &  0x3F));
        test_string[3] = static_cast<char>(0x80 | static_cast<char>(i & 0x3F));
        test_string[4] = test_string[0];
        test_string[5] = test_string[1];
        test_string[6] = test_string[2];
        test_string[7] = test_string[3];

        ret = parse_text_generate_list_characters(test_string, &list_characters_size, 0);
        if (list_characters_size != 1 || ret[0] != i) {
            free(ret);
            return 1000+static_cast<int>(i);
        }
        free(ret);
    }

    return 0;
}
