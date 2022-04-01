/*!
 * \file
 * \brief file kern.c
 *
 * kerning table
 * https://docs.microsoft.com/en-us/typography/opentype/spec/kern
 *
 * Copyright of Timo Hannukkala. All rights reserved.
 *
 * \author Timo Hannukkala <timohannukkala@hotmail.com>
 */

#include "kern.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "glyf.h"
#include "parse_value.h"

static int kern_add_kerning(prj_ttf_reader_data_t *image_data, uint32_t left_character, uint32_t right_character, float kerning);

/*!
 * \brief kern_parse
 *
 * https://docs.microsoft.com/en-us/typography/opentype/spec/kern
 *
 * \param data
 * \param data_size
 * \param offset
 * \param image_data
 * \param rate
 * \param corr_character_table
 * \return
 */
int kern_parse(const uint8_t *data, size_t data_size, size_t offset,
               prj_ttf_reader_data_t *image_data, float rate,
               character_table_t *corr_character_table)
{
    int ret;
    uint16_t version;
    uint16_t index_sub_table, num_sub_tables;
    uint16_t kern_sub_table_version;
    uint16_t kern_sub_table_length;
    uint16_t kern_sub_table_coverage;
    uint16_t num_kern_pairs;
    uint16_t search_range;
    uint16_t entry_selector;
    uint16_t range_shift;
    uint16_t index_kern_pair;
    uint16_t kern_pair_left;
    uint16_t kern_pair_right;
    int16_t kern_pair_value;

    ret = parse_value_16u(data, data_size, &offset, &version);
    if (ret) {
        return ret;
    }

    ret = parse_value_16u(data, data_size, &offset, &num_sub_tables);
    if (ret) {
        return ret;
    }

    for (index_sub_table=0;index_sub_table<num_sub_tables;index_sub_table++) {
        ret = parse_value_16u(data, data_size, &offset, &kern_sub_table_version);
        if (ret) {
            return ret;
        }
        ret = parse_value_16u(data, data_size, &offset, &kern_sub_table_length);
        if (ret) {
            return ret;
        }
        ret = parse_value_16u(data, data_size, &offset, &kern_sub_table_coverage);
        if (ret) {
            return ret;
        }
        ret = parse_value_16u(data, data_size, &offset, &num_kern_pairs);
        if (ret) {
            return ret;
        }
        ret = parse_value_16u(data, data_size, &offset, &search_range);
        if (ret) {
            return ret;
        }
        ret = parse_value_16u(data, data_size, &offset, &entry_selector);
        if (ret) {
            return ret;
        }
        ret = parse_value_16u(data, data_size, &offset, &range_shift);
        if (ret) {
            return ret;
        }

        for (index_kern_pair=0;index_kern_pair<num_kern_pairs;index_kern_pair++) {
            // The glyph index for the left-hand glyph in the kerning pair.
            ret = parse_value_16u(data, data_size, &offset, &kern_pair_left);
            if (ret) {
                return ret;
            }
            // The glyph index for the right-hand glyph in the kerning pair.
            ret = parse_value_16u(data, data_size, &offset, &kern_pair_right);
            if (ret) {
                return ret;
            }

            // The kerning value for the above pair, in FUnits.
            // If this value is greater than zero, the characters will be moved apart.
            // If this value is less than zero, the character will be moved closer together.
            ret = parse_value_16i(data, data_size, &offset, &kern_pair_value);
            if (ret) {
                return ret;
            }

            if (corr_character_table->character[kern_pair_right]
                    && corr_character_table->character[kern_pair_left]
                    && kern_pair_value) {
                ret = kern_add_kerning(image_data, corr_character_table->character[kern_pair_left],
                                       corr_character_table->character[kern_pair_right],
                                       rate*(float)kern_pair_value);
                if (ret) {
                    return ret;
                }
            }
        }
    }

    return 0;
}

/*!
 * \brief kern_add_kerning_right
 *
 * add right characters into left_character list
 *
 * \param left_character
 * \param right_character
 * \param kerning
 * \return 0 on success
 */
static int kern_add_kerning_right(prj_ttf_reader_kerning_left_character_t *left_character, uint32_t right_character, float kerning)
{
    prj_ttf_reader_kerning_right_character_t *tmp;
    tmp = (prj_ttf_reader_kerning_right_character_t *)realloc(left_character->list_right_character,
                                                              sizeof(prj_ttf_reader_kerning_right_character_t)*(left_character->list_right_character_count+1));
    if (!tmp) {
        return errno;
    }

    left_character->list_right_character = tmp;
    left_character->list_right_character[left_character->list_right_character_count].right_character = right_character;
    left_character->list_right_character[left_character->list_right_character_count].kerning = kerning;
    left_character->list_right_character_count++;
    return 0;
}

/*!
 * \brief kern_add_kerning
 *
 * add kerning for left and and right characters
 *
 * \param image_data
 * \param left_character
 * \param right_character
 * \param kerning
 * \return 0 on success
 */
static int kern_add_kerning(prj_ttf_reader_data_t *image_data, uint32_t left_character, uint32_t right_character, float kerning) {
    uint32_t i_left = 0;
    prj_ttf_reader_kerning_left_character_t *tmp;

    for (i_left=0;i_left<image_data->list_kerning_left_character_count;i_left++) {
        if (image_data->list_kerning_left_character[i_left].left_character == left_character) {
            return kern_add_kerning_right(&image_data->list_kerning_left_character[i_left], right_character, kerning);
        }
    }

    tmp = (prj_ttf_reader_kerning_left_character_t *)realloc(image_data->list_kerning_left_character,
                                                             sizeof(prj_ttf_reader_kerning_left_character_t)*(image_data->list_kerning_left_character_count+1));
    if (!tmp) {
        return errno;
    }

    image_data->list_kerning_left_character = tmp;
    image_data->list_kerning_left_character_count++;
    image_data->list_kerning_left_character[i_left].left_character = left_character;
    image_data->list_kerning_left_character[i_left].list_right_character = NULL;
    image_data->list_kerning_left_character[i_left].list_right_character_count = 0;
    return kern_add_kerning_right(&image_data->list_kerning_left_character[i_left], right_character, kerning);
}
