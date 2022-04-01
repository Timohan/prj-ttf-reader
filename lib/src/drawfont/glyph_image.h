/*!
 * \file
 * \brief file glyph_image.h
 *
 * sets glyph into image x/y position
 *
 * Copyright of Timo Hannukkala. All rights reserved.
 *
 * \author Timo Hannukkala <timohannukkala@hotmail.com>
 */

#ifndef GLYPH_IMAGE_H
#define GLYPH_IMAGE_H

#include <stdint.h>
#include "../font_tables.h"
#include "glyph_drawer.h"

int glyph_image_generate_reader_data(uint32_t list_sizes_count, int32_t width, int32_t height, prj_ttf_reader_data_t *data);
int glyph_image_add_glyph_into_image(const font_size_t *list_sizes, int list_index,
                                     const font_drawing_t *drawing, int quality,
                                     prj_ttf_reader_data_t *data, int32_t offset_y);

#endif // GLYPH_IMAGE_H
