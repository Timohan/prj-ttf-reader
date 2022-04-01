/*!
 * \file
 * \brief file glyph_image_positions.h
 *
 * Set glyph's x/y positions into upcoming image of all
 * glyphs
 *
 * Copyright of Timo Hannukkala. All rights reserved.
 *
 * \author Timo Hannukkala <timohannukkala@hotmail.com>
 */

#ifndef GLYPH_IMAGE_POSITIONS_H
#define GLYPH_IMAGE_POSITIONS_H

#include "glyph_image.h"

int glyph_image_positions_generate_glyph_positions(font_size_t *list_sizes, int list_sizes_count, int *required_width, int *required_height);

#endif // GLYPH_IMAGE_POSITIONS_H
