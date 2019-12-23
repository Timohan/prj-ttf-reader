/*!
 * \file
 * \brief file prj-ttf-reader.h
 *
 * Main functions for ttf parsing and so on.
 *
 * Copyright of Timo Hannukkala. All rights reserved.
 *
 * \author Timo Hannukkala <timo.hannukkala@hotmail.com>
 */

#ifndef PRJTTFREADER_H
#define PRJTTFREADER_H

#include <stdint.h>

/*!
 * \brief prj_ttf_reader_image
 *
 * greyscale image data (8bit) that contains the glyphs made by
 * prj_ttf_reader_generate_glyphs_utf8()
 */
typedef struct prj_ttf_reader_image {
    uint8_t *data;      // 8-bit greyscale image data, size is width*height
                        // and pixel position is y*height+x
    int32_t width;      // width of the image
    int32_t height;     // height of the image
} prj_ttf_reader_image_t;

/*!
 * \brief prj_ttf_reader_glyph_data
 *
 * Contains information of glyph's
 * - prj_ttf_reader_image image positions
 * - advance on drawing
 * - bearing on drawing
 *
 * use function prj_ttf_reader_get_character_glyph_data() to get prj_ttf_reader_glyph_data
 * from prj_ttf_reader_data
 */
typedef struct prj_ttf_reader_glyph_data {
    uint32_t character;                 // utf8 character index, for example 'a' == 97

    int32_t image_pixel_left_x;         // left pixel x on the prj_ttf_reader_image_t->data
    int32_t image_pixel_right_x;        // right pixel x on the prj_ttf_reader_image_t->data
    int32_t image_pixel_top_y;          // top pixel y on the prj_ttf_reader_image_t->data
    int32_t image_pixel_bottom_y;       // bottom pixel y on the prj_ttf_reader_image_t->data
    int32_t image_pixel_offset_line_y;  // offset position by pixels when drawing a pixel,
                                        // for example in A or R, it's 0, but with
                                        // characters 'j' or 'y' it can be negative value
                                        // for example, character minus '-' it is
                                        // positive value (pixels to up)
                                        // for example: if 'j' height is 20px, the offset
                                        // can be -4 (4px to down)

    float image_pixel_advance_x;        // advance (how much pixels (x) moves for next character) after the drawing
    float image_pixel_bearing;          // how many pixel (x) moves to right or left side before the drawing
} prj_ttf_reader_glyph_data_t;

/*!
 * \brief prj_ttf_reader_kerning_right_character
 *
 * kerning information of left-right character
 * use function prj_ttf_reader_get_kerning() to get kerning
 */
typedef struct prj_ttf_reader_kerning_right_character {
    uint32_t right_character;
    float kerning;
} prj_ttf_reader_kerning_right_character_t;

/*!
 * \brief prj_ttf_reader_kerning_left_character
 *
 * kerning information of left-right character
 * use function prj_ttf_reader_get_kerning() to get kerning
 */
typedef struct prj_ttf_reader_kerning_left_character {
    uint32_t left_character;

    prj_ttf_reader_kerning_right_character_t *list_right_character;
    uint32_t list_right_character_count;
} prj_ttf_reader_kerning_left_character_t;

/*!
 * \brief prj_ttf_reader_data
 *
 * main reader data that contains parsed ttf data for text string
 * Use functions:
 * - prj_ttf_reader_init_data() to alloc data
 * - prj_ttf_reader_generate_glyphs_utf8() to generate data
 * - prj_ttf_reader_clear_data() to clear data after using
 */
typedef struct prj_ttf_reader_data {
    prj_ttf_reader_image_t image;

    prj_ttf_reader_glyph_data_t *list_data;
    uint32_t list_data_count;

    prj_ttf_reader_kerning_left_character_t *list_kerning_left_character;
    uint32_t list_kerning_left_character_count;
} prj_ttf_reader_data_t;

#ifdef __cplusplus
extern "C" {
#endif
/*!
 * \brief prj_ttf_reader_init_data
 *
 * Allocs the prj_ttf_reader_data_t, makes all values 0
 * Call this function first time
 *
 * \return allocated prj_ttf_reader_data_t with values 0
 */
prj_ttf_reader_data_t *prj_ttf_reader_init_data(void);

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
int prj_ttf_reader_generate_glyphs_utf8(const char *utf8_text, const char *font_file_name, float font_size_px, int quality, prj_ttf_reader_data_t *data);

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
const prj_ttf_reader_glyph_data_t *prj_ttf_reader_get_character_glyph_data(uint32_t character, const prj_ttf_reader_data_t *data);

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
float prj_ttf_reader_get_kerning(uint32_t left_character, uint32_t right_character, const prj_ttf_reader_data_t *data);

/*!
 * \brief prj_ttf_reader_clear_data
 *
 * Clears the data
 * Call this function after prj_ttf_reader_data_t is no longer required to use
 *
 * \param data [in/out] sets data to NULL
 */
void prj_ttf_reader_clear_data(prj_ttf_reader_data_t **data);

/*!
 * \brief prj_ttf_reader_get_character
 *
 * Get first character from the text
 * Example of usage:
 * uint32_t *list_characters, list_characters_size;
 * prj_ttf_reader_get_characters("ગુજરાતી યુનિકોડ ફોન્ટ સૂચી", &list_characters, &list_characters_size);
 * ...  ...
 * free(list_characters);
 *
 * \param utf8_text [in] utf8 text
 * \param list_characters [out] list characters from text, first character is on [0]
 * \param list_characters_size [out] the list_characters size, count of characters
 * \return 0 on success
 */
int prj_ttf_reader_get_characters(const char *utf8_text, uint32_t **list_characters, uint32_t *list_characters_size);
#ifdef __cplusplus
}
#endif

#endif // PRJTTFREADER_H
