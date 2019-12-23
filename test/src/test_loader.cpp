/*!
* \file
* \brief file test_loader.cpp
*
* Load unit test files for testing
*
* Copyright of Timo Hannukkala, All rights reserved.
*
* \author Timo Hannukkala <timohannukkala@hotmail.com>
*/

#include "test_loader.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "../../lib/src/drawfont/glyph_drawer.h"

/*!
 * \brief parse_values
 *
 * parse values from image.txt line
 *
 * \param buffer
 * \param bufferString
 * \param retValue0
 * \param retValue1
 * \return 0 on success
 */
int parse_values(const char *buffer, const char *bufferString, size_t *retValue0, size_t *retValue1) {
    if (!strstr(buffer, "/")) {
        return 1;
    }

    int value0, value1;

    value0 = atoi(buffer+strlen(bufferString));
    if (value0 < 0) {
        return 2;
    }

    value1 = atoi(strstr(buffer, "/")+1);
    if (value1 < 0) {
        return 3;
    }

#ifdef __cplusplus
    *retValue0 = static_cast<size_t>(value0);
    *retValue1 = static_cast<size_t>(value1);
#else
    *retValue0 = (size_t)value0;
    *retValue1 = (size_t)value1;
#endif
    return 0;
}

/*!
 * \brief test_getpixel_drawing
 * \param x
 * \param y
 * \param width
 * \param listpixel_drawings
 * \return
 */
pixel_drawing_t *test_getpixel_drawing(size_t x, size_t y, size_t width, pixel_drawing_t *listpixel_drawings) {
    return &listpixel_drawings[y*width+x];
}

void clear_pixel_list(pixel_drawing_t **listpixel_drawings) {
#ifdef __cplusplus
    delete[] *listpixel_drawings;
#else
    free(*listpixel_drawings);
#endif
}

/*!
 * \brief tst_rotate
 *
 * rotate coordinates
 *
 * \param x
 * \param y
 * \param xOut returns rotated coordinate x
 * \param yOut returns rotated coordinate y
 * \param original_width
 * \param original_height
 * \param rotate
 */
void tst_rotate(size_t x, size_t y, size_t *xOut, size_t *yOut, size_t original_width, size_t original_height, LoadTestImageRotation rotate)
{
    ssize_t xTmp[2];
    ssize_t yTmp[2];
    size_t xx;
    size_t yy;
    switch (rotate) {
        case LoadTestImageRotationNoRotate:
            *xOut = x;
            *yOut = y;
            break;
        case LoadTestImageRotationUpsideDown:
            *xOut = x;
            *yOut = original_height-y-1;
            break;
        case LoadTestImageRotation90ClockWise:
            xTmp[0] = static_cast<ssize_t>(x) - static_cast<ssize_t>(original_width)/2;
            yTmp[0] = static_cast<ssize_t>(original_height-y-1) - static_cast<ssize_t>(original_height)/2;
            if (xTmp[0] < 0 && yTmp[0] < 0) {
                xTmp[1] = yTmp[0];
                yTmp[1] = xTmp[0]*-1;
            } else if (xTmp[0] < 0 && yTmp[0] >= 0) {
                xTmp[1] = yTmp[0];
                yTmp[1] = xTmp[0]*-1;
            } else if (xTmp[0] >= 0 && yTmp[0] >= 0) {
                xTmp[1] = yTmp[0];
                yTmp[1] = xTmp[0]*-1;
            } else {
                xTmp[1] = yTmp[0];
                yTmp[1] = xTmp[0]*-1;
            }
            *xOut = static_cast<size_t>(static_cast<ssize_t>(original_width)/2 + xTmp[1]);
            *yOut = original_height-static_cast<size_t>(static_cast<ssize_t>(original_height)/2 + yTmp[1])-1;
            break;
        case LoadTestImageRotation90CounterClockWise:
            tst_rotate(x, y, &xx, &yy, original_width, original_height, LoadTestImageRotation90ClockWise);
            *xOut = original_width-xx-1;
            *yOut = original_height-yy-1;
            break;
        defaut:
            break;
    }
}

/*!
 * \brief load_test_image
 *
 * load image.txt and create pixelsOut
 *
 * \param fileName image.txt filename
 * \param pixelsOut
 * \param width width of the pixelsOut
 * \param height height of the pixelsOut
 * \param rotate rotation
 * \return 0 on success
 */
int load_test_image(const char *fileName, pixel_drawing_t **pixelsOut, size_t *width, size_t *height, LoadTestImageRotation rotate) {
    int ret = 0;
    char buffer[124];
    FILE *fp = fopen(fileName, "r");
    if (!fp) {
        return 1;
    }

    int sizeDone = 0;
    size_t val0, val1;
    const char *stringSize = "SIZE: ";
    const char *stringInnerpixel_drawing = "INNER: ";
    const char *stringLine = "LINE: ";

    while (ret == 0) {
        if (!fgets(buffer, sizeof(buffer), fp)) {
            break;
        }

        if (strncmp(buffer, stringSize, strlen(stringSize)) == 0) {
            ret = parse_values(buffer, stringSize, &val0, &val1);
            if (ret != 0) {
                break;
            }
            if (!val0 || !val1) {
                ret = 4;
                break;
            }

            if (rotate == LoadTestImageRotationNoRotate
                    || rotate == LoadTestImageRotationUpsideDown) {
                *width = val0;
                *height = val1;
            } else {
                *width = val1;
                *height = val0;
            }
            sizeDone = 1;
#ifdef __cplusplus
            *pixelsOut = new pixel_drawing_t[val0*val1];
#else
            *pixelsOut = (pixel_drawing *)malloc(val0*val1*sizeof(pixel_drawing));
 #endif
            memset(*pixelsOut, 0, val0*val1*sizeof(pixel_drawing_t));
        } else if (strncmp(buffer, stringInnerpixel_drawing, strlen(stringInnerpixel_drawing)) == 0) {
            if (sizeDone == 0) {
                ret = 6;
                break;
            }
            ret = parse_values(buffer, stringSize, &val0, &val1);
            if (ret != 0) {
                ret += 10;
                break;
            }

            switch (rotate) {
                case LoadTestImageRotationNoRotate:
                default:
                    test_getpixel_drawing(val0, val1, *width, *pixelsOut)->inner_pixel = true;
                    break;
                case LoadTestImageRotationUpsideDown:
                    test_getpixel_drawing(val0, *height-val1-1, *width, *pixelsOut)->inner_pixel = true;
                    break;
            }

        } else if (strncmp(buffer, stringLine, strlen(stringLine)) == 0) {
            if (sizeDone == 0) {
                ret = 7;
                break;
            }
            ret = parse_values(buffer, stringSize, &val0, &val1);
            if (ret != 0) {
                ret += 100;
                break;
            }
            switch (rotate) {
                case LoadTestImageRotationNoRotate:
                default:
                    test_getpixel_drawing(val0, val1, *width, *pixelsOut)->line = 1;
                    break;
                case LoadTestImageRotationUpsideDown:
                    test_getpixel_drawing(val0, *height-val1-1, *width, *pixelsOut)->line = 1;
                    break;
                case LoadTestImageRotation90ClockWise:
                    test_getpixel_drawing(val0, val1, *width, *pixelsOut)->line = 1;
                    break;
            }
        }

    }
    fclose(fp);

    if (!ret && !sizeDone) {
        return 5;
    }

    if (ret && sizeDone) {
        clear_pixel_list(pixelsOut);
        return 8;
    }

    return ret;
}
