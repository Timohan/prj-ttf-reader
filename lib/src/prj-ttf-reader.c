/*!
 * \file
 * \brief file prjttfreader.c
 *
 * Main functions for ttf parsing and so on.
 *
 * Copyright of Timo Hannukkala. All rights reserved.
 *
 * \author Timo Hannukkala <timo.hannukkala@hotmail.com>
 */

#include "prj-ttf-reader.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include "drawfont/glyph_drawer.h"
#include "drawfont/glyph_filler.h"
#include "drawfont/glyph_image.h"
#include "drawfont/glyph_image_positions.h"
#include "drawfont/glyph_graph_generator.h"
#include "reader/parse_value.h"
#include "reader/parse_text.h"
#include "font_tables.h"
#include "supported_characters/read_supported_characters.h"

static int prj_ttf_reader_parse_data(const uint32_t *list_characters, uint32_t list_characters_size,
                                     const uint8_t *data, size_t data_size, float font_size_px, font_tables_t *tables,
                                     int quality, prj_ttf_reader_data_t *image_data);
static uint8_t *prj_ttf_reader_read_file(const char* file_name, size_t *file_data_size);
static void prj_ttf_reader_clear(font_tables_t *tables);
static int prj_ttf_reader_generate_glyphs_from_list(const uint32_t *list_characters, uint32_t list_characters_size,
                                             const char *font_file_name, float font_size_px, int quality,
                                             prj_ttf_reader_data_t *data);

/*!
 * \brief prj_ttf_reader_init_data
 *
 * Allocs the prj_ttf_reader_data_t, makes all values 0
 * Call this function first time
 *
 * \return allocated prj_ttf_reader_data_t with values 0
 */
prj_ttf_reader_data_t *prj_ttf_reader_init_data(void)
{
    return (prj_ttf_reader_data_t *)calloc(1, sizeof(prj_ttf_reader_data_t));
}

/*!
 * \brief prj_ttf_reader_clear_data
 *
 * Clears the data
 * Call this function after prj_ttf_reader_data_t is no longer required to use
 *
 * \param data [in/out] sets data to NULL
 */
void prj_ttf_reader_clear_data(prj_ttf_reader_data_t **data)
{
    uint32_t i;
    if (!*data) {
        return;
    }

    free((*data)->list_data);
    free((*data)->image.data);

    for (i=0;i<(*data)->list_kerning_left_character_count;i++) {
        free((*data)->list_kerning_left_character[i].list_right_character);
    }
    free((*data)->list_kerning_left_character);

    free(*data);
    *data = NULL;
}

/*!
 * \brief prj_ttf_reader_generate_glyphs_utf8
 *
 * generates the glyph(s) images from the list of characters (of utf8_text)
 * prj_ttf_reader_init_data() must be called before this function
 *
 * \param utf8_text [in]
 * \param font_file_name [in] full filepath of ttf file
 * \param font_size_px [in] font size's in px
 * \param quality [in] quality of the anti-aliasing, use 5 or 10 (5 is faster than 10)
 * \param data [in/out] fills the prj_ttf_reader_data_t, this data was got from prj_ttf_reader_init_data
 * \return 0 on success
 */
int prj_ttf_reader_generate_glyphs_utf8(const char *utf8_text, const char *font_file_name, float font_size_px, int quality, prj_ttf_reader_data_t *data)
{
    int ret;
    uint32_t list_characters_size;
    uint32_t *list_characters = parse_text_generate_list_characters(utf8_text, &list_characters_size, 0);
    if (!list_characters || !list_characters_size) {
        return 1;
    }

    ret = prj_ttf_reader_generate_glyphs_from_list(list_characters, list_characters_size,
                                                   font_file_name, font_size_px, quality,
                                                   data);

    free(list_characters);
    return ret;
}

/*!
 * \brief prj_ttf_reader_generate_glyphs_from_list
 *
 * generates the glyph(s) images from the list of characters
 *
 * \param list_characters
 * \param list_characters_size
 * \param font_file_name
 * \param font_size_px
 * \param quality
 * \param data
 * \return
 */
static int prj_ttf_reader_generate_glyphs_from_list(const uint32_t *list_characters, uint32_t list_characters_size,
                                             const char *font_file_name, float font_size_px, int quality,
                                             prj_ttf_reader_data_t *data)
{
    font_tables_t tables;
    memset(&tables, 0, sizeof(tables));

    uint8_t *file_data;
    size_t file_data_size;
    int ret;

    file_data = prj_ttf_reader_read_file(font_file_name, &file_data_size);

    if (!file_data) {
        return 1;
    }

    ret = prj_ttf_reader_parse_data(list_characters, list_characters_size, file_data, file_data_size, font_size_px, &tables, quality, data);
    free(file_data);
    prj_ttf_reader_clear(&tables);
    return ret;
}

/*!
 * \brief clear
 *
 * Clears the font_tables
 *
 * \param tables
 */
static void prj_ttf_reader_clear(font_tables_t *tables)
{
    glyf_clear(&tables->list_glyph, &tables->max_profile);
    name_clear(&tables->name_table);
    loca_clear(&tables->list_index_loc_to_tables);
    cmap_clear(&tables->character_to_glyph_index_table);
    hmtx_clear(&tables->hor_metrics_table);
    otff_clear(&tables->list_table_record);
    free(tables->list_font_sizes);
    tables->list_font_sizes_count = 0;
}

/*!
 * \brief prj_ttf_reader_read_file
 *
 * Reads the file
 *
 * \param file_name
 * \param file_data_size
 * \return
 */
static uint8_t *prj_ttf_reader_read_file(const char* file_name, size_t *file_data_size)
{
    uint8_t *file_data;
    size_t file_size, i;
    FILE *fp = fopen(file_name, "r");
    if (!fp) {
        *file_data_size = 0;
        return NULL;
    }

    fseek(fp, 0L, SEEK_END);
    file_size = (size_t)(ftell(fp));
    if (!file_size) {
        *file_data_size = 0;
        fclose(fp);
        return NULL;
    }


    file_data = (uint8_t *)malloc(file_size);
    fseek(fp, 0L, SEEK_SET);
    i = fread(file_data, 1, file_size, fp);
    if (i != file_size) {
        free(file_data);
        file_data = NULL;
        *file_data_size = 0;
    } else {
        *file_data_size = file_size;
    }
    fclose(fp);
    return file_data;
}

/*!
 * \brief prj_ttf_reader_parse_data
 *
 * parse the ttf data
 *
 * \param list_characters
 * \param list_characters_size
 * \param data
 * \param data_size
 * \param font_size_px
 * \param tables
 * \param quality
 * \param data
 * \return 0 on success
 */
static int prj_ttf_reader_parse_data(const uint32_t *list_characters, uint32_t list_characters_size,
                                     const uint8_t *data, size_t data_size, float font_size_px,
                                     font_tables_t *tables, int quality, prj_ttf_reader_data_t *image_data)
{
    int ret;
    uint16_t i;
    uint16_t index_kern;
    size_t offset = 0;

    ret = otff_parse_offset_table(data, data_size, &offset, &tables->offsets);
    if (ret) {
        return ret;
    }

    ret = otff_parse_table_records(data, data_size, &offset, &tables->list_table_record, &tables->offsets);
    if (ret) {
        return ret;
    }

    i = otff_get_table_record_index(tables->list_table_record, tables->offsets.num_tables, "head");
    if (UINT16_MAX == i) {
        return EINVAL;
    }

    ret = head_parse_head_table(data, data_size, tables->list_table_record[i].offset, &tables->header_table);
    if (ret) {
        return ret;
    }

    i = otff_get_table_record_index(tables->list_table_record, tables->offsets.num_tables, "maxp");
    if (UINT16_MAX == i) {
        return EINVAL;
    }

    ret = maxp_parse_maximum_profile(data, data_size, tables->list_table_record[i].offset, &tables->max_profile);
    if (ret) {
        return ret;
    }

    i = otff_get_table_record_index(tables->list_table_record, tables->offsets.num_tables, "name");
    if (UINT16_MAX == i) {
        return EINVAL;
    }

    ret = name_parse_naming_table(data, data_size, tables->list_table_record[i].offset, &tables->name_table);
    if (ret) {
        return ret;
    }

    i = otff_get_table_record_index(tables->list_table_record, tables->offsets.num_tables, "loca");
    if (UINT16_MAX == i) {
        return EINVAL;
    }

    if (tables->max_profile.glyphs_count == 0) {
        return EINVAL;
    }

    ret = loca_parse(data, data_size, tables->list_table_record[i].offset, &tables->list_index_loc_to_tables,
                     (uint16_t)(tables->max_profile.glyphs_count+1), tables->header_table.format);
    if (ret) {
        return ret;
    }


    i = otff_get_table_record_index(tables->list_table_record, tables->offsets.num_tables, "cmap");
    if (UINT16_MAX == i) {
        return EINVAL;
    }

    ret = cmap_parse_character_to_glyph_index_mapping_table(data, data_size, tables->list_table_record[i].offset,
                                                            &tables->character_to_glyph_index_table, &tables->corr_character_table);
    if (ret) {
        return ret;
    }

    i = otff_get_table_record_index(tables->list_table_record, tables->offsets.num_tables, "hhea");
    if (UINT16_MAX == i) {
        return EINVAL;
    }

    ret = hhea_parse(data, data_size, tables->list_table_record[i].offset, &tables->hor_header_table);
    if (ret) {
        return ret;
    }

    i = otff_get_table_record_index(tables->list_table_record, tables->offsets.num_tables, "hmtx");
    if (UINT16_MAX == i) {
        return EINVAL;
    }


    ret = hmtx_parse(data, data_size,
                     &tables->hor_metrics_table, tables->list_table_record[i].offset,
                     &tables->hor_header_table, &tables->max_profile);
    if (ret) {
        return ret;
    }


    ret = glyph_graph_generator_generate_graph(list_characters, list_characters_size,
                                               data, data_size, font_size_px, tables, quality, image_data,
                                               &tables->hor_metrics_table, &tables->hor_header_table);
    if (ret) {
        return ret;
    }

    index_kern = otff_get_table_record_index(tables->list_table_record, tables->offsets.num_tables, "kern");
    //Kearning table
    if (UINT16_MAX != index_kern) {
        ret = kern_parse(data, data_size, tables->list_table_record[index_kern].offset,
                         image_data, (float)font_size_px/tables->header_table.units_per_em,
                         &tables->corr_character_table);
        if (ret) {
            return ret;
        }
    }

#ifdef TIME_DEBUG
    print_glyph_filler_times();
#endif
    return 0;
}

/*!
 * \brief prj_ttf_reader_get_kerning
 *
 * Get kerning between characters
 *
 * \param left_character left character index, for example 'a' == 97
 * \param right_character right character index, for example 'b' == 98
 * \param data data that was generated by prj_ttf_reader_generate_glyphs_utf8()
 * \return kerning
 */
float prj_ttf_reader_get_kerning(uint32_t left_character, uint32_t right_character, const prj_ttf_reader_data_t *data)
{
    uint32_t i_left, i_right;

    if (!data->list_kerning_left_character) {
        return 0;
    }

    for (i_left=0;i_left<data->list_kerning_left_character_count;i_left++) {
        if (data->list_kerning_left_character[i_left].left_character == left_character) {
            for (i_right=0;i_right<data->list_kerning_left_character[i_left].list_right_character_count;i_right++) {
                if (data->list_kerning_left_character[i_left].list_right_character[i_right].right_character == right_character) {
                    return data->list_kerning_left_character[i_left].list_right_character[i_right].kerning;
                }
            }
            break;
        }
    }

    return 0;
}

/*!
 * \brief prj_ttf_reader_get_character_glyph_data
 *
 * Get pointer of character data from data
 * prj_ttf_reader_generate_glyphs_utf8() must be called before calling this function
 *
 * character indexes for utf8 text can be found by using function prj_ttf_reader_get_characters
 *
 * \param character to find, for example 'a' == 97
 * \param data find character from data
 * \return NULL if the character was not found, otherwise returns pointer to prj_ttf_reader_glyph_data_t
 */
const prj_ttf_reader_glyph_data_t *prj_ttf_reader_get_character_glyph_data(uint32_t character, const prj_ttf_reader_data_t *data)
{
    uint32_t i;
    for (i=0;i<data->list_data_count;i++) {
        if (data->list_data[i].character == character) {
            return &data->list_data[i];
        }
    }
    return NULL;
}

/*!
 * \brief prj_ttf_reader_get_character
 *
 * Get character list from the utf8_text
 * Example of usage:
 * uint32_t *list_characters, list_characters_size;
 * prj_ttf_reader_get_characters("ગુજરાતી યુનિકોડ ફોન્ટ સૂચી", &list_characters, &list_characters_size);
 * ... ...
 * free(list_characters);
 *
 * \param utf8_text [in] utf8 text
 * \param list_characters [out] list characters from text, first character is on [0]
 * \param list_characters_size [out] the list_characters size, count of characters
 * \return 0 on success
 */
int prj_ttf_reader_get_characters(const char *utf8_text, uint32_t **list_characters, uint32_t *list_characters_size)
{
    uint32_t *ret = parse_text_generate_list_characters(utf8_text, list_characters_size, 1);
    if (!ret) {
        *list_characters_size = 0;
        return -1;
    }

    *list_characters = ret;
    return 0;
}

/*!
 * \brief prj_ttf_reader_init_supported_character
 *
 * Allocs the prj_ttf_reader_supported_characters_t,
 * makes all values 0 or NULL
 * Call this function first time
 *
 * \return allocated prj_ttf_reader_supported_characters_t with values 0
 */
prj_ttf_reader_supported_characters_t *prj_ttf_reader_init_supported_character(void)
{
    return (prj_ttf_reader_supported_characters_t *)calloc(1, sizeof(prj_ttf_reader_supported_characters_t));
}

/*!
 * \brief prj_ttf_reader_clear_supported_character
 *
 * Clears the supported characters data
 * Call this function after prj_ttf_reader_supported_characters_t is no longer required to use
 *
 * \param data [in/out] sets data to NULL
 */
void prj_ttf_reader_clear_supported_character(prj_ttf_reader_supported_characters_t **supported_characters)
{
    if (!*supported_characters) {
        return;
    }

    if ((*supported_characters)->list_character) {
        free((*supported_characters)->list_character);
    }
    free(*supported_characters);
    *supported_characters = NULL;
}

/*!
 * \brief prj_ttf_reader_get_supported_characters
 *
 * get supported characters from font file (ttf)
 *
 * \param font_file_name [in] full filepath of ttf file
 * \param supported_characters [out] supported characters will be filled here
 * \return 0 on success
 */
int prj_ttf_reader_get_supported_characters(const char *font_file_name, prj_ttf_reader_supported_characters_t *supported_characters)
{
    font_tables_t tables;
    memset(&tables, 0, sizeof(tables));

    uint8_t *file_data;
    size_t file_data_size;
    int ret;

    file_data = prj_ttf_reader_read_file(font_file_name, &file_data_size);

    if (!file_data) {
        return 1;
    }

    ret = prj_ttf_reader_parse_supported_characters(file_data, file_data_size, &tables, supported_characters);

    free(file_data);
    prj_ttf_reader_clear(&tables);
    return ret;
}
