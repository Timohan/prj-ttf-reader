/*!
* \file
* \brief file test_loader.h
*
* Load unit test files for testing
*
* Copyright of Timo Hannukkala, All rights reserved.
*
* \author Timo Hannukkala <timohannukkala@hotmail.com>
*/

#ifndef TEST_LOADER_H
#define TEST_LOADER_H

#include <stddef.h>
#include "../../lib/src/drawfont/glyph_drawer.h"

enum LoadTestImageRotation {
    LoadTestImageRotationNoRotate,
    LoadTestImageRotationUpsideDown,
    LoadTestImageRotation90ClockWise,
    LoadTestImageRotation90CounterClockWise,
};

pixel_drawing_t *test_getpixel_drawing(size_t x, size_t y, size_t width, pixel_drawing_t *listpixel_drawings);
void clear_pixel_list(pixel_drawing_t **listpixel_drawings);
void tst_rotate(size_t x, size_t y, size_t *xOut, size_t *yOut, size_t original_width, size_t original_height, LoadTestImageRotation rotate);
int load_test_image(const char *fileName, pixel_drawing_t **pixelsOut, size_t *width, size_t *height, LoadTestImageRotation rotate);

#endif // TEST_LOADER_H
