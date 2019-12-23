/*!
* \file
* \brief file glyph_drawer.cpp
*
* glyph_drawer unit tests
*
* Copyright of Timo Hannukkala, All rights reserved.
*
* \author Timo Hannukkala <timohannukkala@hotmail.com>
*/
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include "tst_glyph_drawer.h"
#include "../../lib/src/drawfont/glyph_drawer.h"
#include "../../lib/src/drawfont/glyph_filler.h"
#include "test_loader.h"
#include "tst_glyph_filler.h"

/*!
 * \brief tst_set_min_max_font_drawing_values
 *
 * Helper function, set's drawer's min/max x/y
 *
 */
void tst_set_min_max_font_drawing_values(font_drawing_t *drawer)
{
    int x, y;
    for (x=0;x<drawer->width;x++) {
        for (y=0;y<drawer->height;y++) {
            if (drawer->list_pixels[y*drawer->width+x].line) {
                if (!drawer->line_max_x && !drawer->line_max_y) {
                    drawer->line_min_x = x;
                    drawer->line_max_x = x;
                    drawer->line_min_y = y;
                    drawer->line_max_y = y;
                } else {
                    if (drawer->line_min_x > x) {
                        drawer->line_min_x = x;
                    }
                    if (drawer->line_min_y > y) {
                        drawer->line_min_y = y;
                    }
                    if (drawer->line_max_x < x) {
                        drawer->line_max_x = x;
                    }
                    if (drawer->line_max_y < y) {
                        drawer->line_max_y = y;
                    }
                }
            }
        }
    }
}

/*!
 * \brief tst_fillInnerAreaInImageFilesRotate
 *
 * drawInnerArea() testing
 *
 * \return
 */
int tst_fillInnerAreaInImageFilesRotate(LoadTestImageRotation rotation)
{
    pixel_drawing_t *pixelsCorrect = nullptr;
    size_t width = 0, height = 0, x, y;
    int ret;
    char *fileName = new char[strlen(TEST_IMAGE_FOLDERS) + 100];
    DIR *dirStream;
    font_drawing_t drawer;

    struct dirent *dir;
    dirStream = opendir(TEST_IMAGE_FOLDERS);
    ret = -1;
    if (dirStream) {
        while (1) {
            dir = readdir(dirStream);
            if (!dir) {
                break;
            }

            if (!strstr(dir->d_name, "_image.fdi")) {
                continue;
            }

            // must have at least "_image.fdi" file
            if (ret == -1) {
                ret = 0;
            }

            sprintf(fileName, "%s/%s", TEST_IMAGE_FOLDERS, dir->d_name);
            ret = load_test_image(fileName, &pixelsCorrect, &width, &height, rotation);
            if (ret) {
                delete[] fileName;
                return ret;
            }

            memset(&drawer, 0, sizeof(drawer));
            glyph_drawer_init(&drawer, static_cast<int>(width), static_cast<int>(height));
            for (x=0;x<width;x++) {
                for (y=0;y<height;y++) {
                    drawer.list_pixels[y*drawer.width+x].line = test_getpixel_drawing(x, y, width, pixelsCorrect)->line;
                }
            }

            tst_set_min_max_font_drawing_values(&drawer);
            glyph_filler_draw_inner_area(&drawer);

            for (x=0;x<width;x++) {
                for (y=0;y<height;y++) {
                    if (drawer.list_pixels[y*drawer.width+x].line != test_getpixel_drawing(x, y, width, pixelsCorrect)->line) {
                        clear_pixel_list(&pixelsCorrect);
                        delete[] fileName;
                        return 1;
                    }
                    if (drawer.list_pixels[y*drawer.width+x].inner_pixel != test_getpixel_drawing(x, y, width, pixelsCorrect)->inner_pixel) {
                        clear_pixel_list(&pixelsCorrect);
                        delete[] fileName;
                        return 2;
                    }
                }
            }

            clear_pixel_list(&pixelsCorrect);
            glyph_drawer_clear(&drawer);
        }
        closedir(dirStream);
    }

    delete[] fileName;

    if (ret) {
        return ret;
    }

    return 0;
}

/*!
 * \brief tst_fillInnerAreaInImageFiles
 *
 * drawInnerArea() testing
 *
 * \return
 */
int tst_fillInnerAreaInImageFiles()
{
    int ret;

    ret = tst_fillInnerAreaInImageFilesRotate(LoadTestImageRotationNoRotate);
    if (ret) {
        return ret;
    }
    ret = tst_fillInnerAreaInImageFilesRotate(LoadTestImageRotationUpsideDown);
    if (ret) {
        return ret;
    }

    ret = tst_fillInnerAreaInImageFilesRotate(LoadTestImageRotation90ClockWise);
    if (ret) {
        return ret;
    }

    ret = tst_fillInnerAreaInImageFilesRotate(LoadTestImageRotation90CounterClockWise);
    if (ret) {
        return ret;
    }
    return ret;
}

