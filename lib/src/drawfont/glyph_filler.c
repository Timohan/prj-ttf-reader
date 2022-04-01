/*!
 * \file
 * \brief file glyph_filler.c
 *
 * Fills the glyph graphics
 *
 * Copyright of Timo Hannukkala. All rights reserved.
 *
 * \author Timo Hannukkala <timohannukkala@hotmail.com>
 */

#include "glyph_filler.h"
#include "glyph_drawer.h"
#ifdef TIME_DEBUG
#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#endif

/*!
 * \brief glyph_filler_get_pixel
 * \param drawing
 * \param x
 * \param y
 * \return
 */
pixel_drawing_t *glyph_filler_get_pixel(font_drawing_t *drawing, int x, int y)
{
    return &drawing->list_pixels[y*drawing->width+x];
}

/*!
 * \brief glyph_filler_enlarge_inner_area
 *
 * enlarge inner area
 *
 * \param drawing
 * \return
 */
static uint8_t glyph_filler_enlarge_inner_area(font_drawing_t *drawing)
{
    int x_plus;
    int y_plus;
    int x, y;
    int pixel_index;
    pixel_drawing_t *pixel;
    uint8_t increased = 0;

    for (y=drawing->line_min_y+1;y<drawing->line_max_y;y++) {
        pixel_index = y*drawing->width + drawing->line_min_x + 1;
        for (x=drawing->line_min_x+1;x<drawing->line_max_x;x++) {
            if (drawing->list_pixels[pixel_index].completed_y
                    || !drawing->list_pixels[pixel_index].inner_pixel) {
                pixel_index++;
                continue;
            }

            drawing->list_pixels[pixel_index].completed_y = 1;
            for (y_plus=y+1;y_plus<drawing->line_max_y;y_plus++) {
                pixel = glyph_filler_get_pixel(drawing, x, y_plus);
                if (pixel->line || pixel->inner_pixel) {
                    break;
                }

                pixel->completed_y = 1;
                pixel->inner_pixel = 1;
                increased = 1;
            }

            for (y_plus=y-1;y_plus>drawing->line_min_y;y_plus--) {
                pixel = glyph_filler_get_pixel(drawing, x, y_plus);
                if (pixel->line || pixel->inner_pixel) {
                    break;
                }

                pixel->completed_y = 1;
                pixel->inner_pixel = 1;
                increased = 1;
            }
            pixel_index++;
        }
    }

    for (y=drawing->line_min_y+1;y<drawing->line_max_y;y++) {
        pixel_index = y*drawing->width + drawing->line_min_x + 1;
        for (x=drawing->line_min_x+1;x<drawing->line_max_x;x++) {
            if (drawing->list_pixels[pixel_index].completed_x
                    || !drawing->list_pixels[pixel_index].inner_pixel) {
                pixel_index++;
                continue;
            }

            drawing->list_pixels[pixel_index].completed_x = 1;
            for (x_plus=x+1;x_plus<drawing->line_max_x;x_plus++) {
                pixel = glyph_filler_get_pixel(drawing, x_plus, y);
                if (pixel->line || pixel->inner_pixel) {
                    break;
                }

                pixel->completed_x = 1;
                pixel->inner_pixel = 1;
                increased = 1;
            }

            for (x_plus=x-1;x_plus>drawing->line_min_x;x_plus--) {
                pixel = glyph_filler_get_pixel(drawing, x_plus, y);
                if (pixel->line || pixel->inner_pixel) {
                    break;
                }

                pixel->completed_x = 1;
                pixel->inner_pixel = 1;
                increased = 1;
            }
            pixel_index++;
        }
    }

    return increased;
}

static void glyph_filler_enlarge_outer_area_first(font_drawing_t *drawing)
{
    int x, y;
    int pixel_index;

    int start_x = drawing->line_min_x;
    int end_x = drawing->line_max_x;
    int start_y = drawing->line_min_y;
    int end_y = drawing->line_max_y;

    if (start_x > 0) {
        start_x--;
    }
    if (start_y > 0) {
        start_y--;
    }
    if (end_x < drawing->width-1) {
        end_x++;
    }
    if (end_y < drawing->height-1) {
        end_y++;
    }

    for (y=start_y;y<end_y;y++) {
        pixel_index = y*drawing->width + start_x;
        for (x=start_x;x<end_x;x++) {
            if (!drawing->list_pixels[pixel_index].line) {
                drawing->list_pixels[pixel_index].outer_pixel = 1;
                if (x > start_x) {
                    drawing->list_pixels[pixel_index-1].completed_x = 1;
                }
            } else {
                break;
            }
            pixel_index++;
        }
    }

    for (x=start_x;x<end_x;x++) {
        for (y=start_y;y<end_y;y++) {
            pixel_index = y*drawing->width + x;
            if (!drawing->list_pixels[pixel_index].line) {
                drawing->list_pixels[pixel_index].outer_pixel = 1;
                if (y > start_y) {
                    drawing->list_pixels[pixel_index-drawing->width].completed_y = 1;
                }
            } else {
                break;
            }
        }
    }

    for (y=start_y;y<end_y;y++) {
        pixel_index = y*drawing->width + end_x-1;
        for (x=end_x-1;x>=start_x;x--) {
            if (!drawing->list_pixels[pixel_index].line) {
                drawing->list_pixels[pixel_index].outer_pixel = 1;
                if (x < end_x-1) {
                    drawing->list_pixels[pixel_index+1].completed_x = 1;
                }
            } else {
                break;
            }
            pixel_index--;
        }
    }

    for (x=start_x;x<end_x;x++) {
        for (y=end_y-1;y>=start_y;y--) {
            pixel_index = y*drawing->width + x;
            if (!drawing->list_pixels[pixel_index].line) {
                drawing->list_pixels[pixel_index].outer_pixel = 1;
                if (y < end_y-1) {
                    drawing->list_pixels[pixel_index+drawing->width].completed_y = 1;
                }
            } else {
                break;
            }
        }
    }
}


/*!
 * \brief glyph_filler_enlarge_outer_area
 *
 * enlarge outer area
 *
 * \param drawing
 * \return
 */
static uint8_t glyph_filler_enlarge_outer_area(font_drawing_t *drawing)
{
    int x_plus;
    int y_plus;
    int x, y;
    int pixel_index;
    pixel_drawing_t *pixel;
    uint8_t increased = 0;

    int start_x = drawing->line_min_x;
    int end_x = drawing->line_max_x;
    int start_y = drawing->line_min_y;
    int end_y = drawing->line_max_y;

    if (start_x > 0) {
        start_x--;
    }
    if (start_y > 0) {
        start_y--;
    }
    if (end_x < drawing->width-1) {
        end_x++;
    }
    if (end_y < drawing->height-1) {
        end_y++;
    }

    for (y=start_y;y<end_y;y++) {
        for (x=start_x;x<end_x;x++) {
            pixel_index = y*drawing->width + x;
            if (drawing->list_pixels[pixel_index].completed_y) {
                continue;
            }


            if (drawing->list_pixels[pixel_index].outer_pixel) {
                drawing->list_pixels[pixel_index].completed_y = 1;
                for (y_plus=y+1;y_plus<end_y;y_plus++) {
                    pixel = glyph_filler_get_pixel(drawing, x, y_plus);
                    if (pixel->line || pixel->outer_pixel) {
                        break;
                    }

                    pixel->completed_y = 1;
                    pixel->outer_pixel = 1;
                    increased = 1;
                }

                for (y_plus=y-1;y_plus>=start_y;y_plus--) {
                    pixel = glyph_filler_get_pixel(drawing, x, y_plus);
                    if (pixel->line || pixel->outer_pixel) {
                        break;
                    }

                    pixel->completed_y = 1;
                    pixel->outer_pixel = 1;
                    increased = 1;
                }
            }
        }
    }

    for (x=start_x;x<end_x;x++) {
        for (y=start_y;y<end_y;y++) {
            pixel_index = y*drawing->width + x;
            if (drawing->list_pixels[pixel_index].completed_x) {
                continue;
            }


            if (drawing->list_pixels[pixel_index].outer_pixel) {
                drawing->list_pixels[pixel_index].completed_x = 1;
                for (x_plus=x+1;x_plus<end_x;x_plus++) {
                    pixel = glyph_filler_get_pixel(drawing, x_plus, y);
                    if (pixel->line || pixel->outer_pixel) {
                        break;
                    }

                    pixel->completed_x = 1;
                    pixel->outer_pixel = 1;
                    increased = 1;
                }

                for (x_plus=x-1;x_plus>=start_x;x_plus--) {
                    pixel = glyph_filler_get_pixel(drawing, x_plus, y);
                    if (pixel->line || pixel->outer_pixel) {
                        break;
                    }

                    pixel->completed_x = 1;
                    pixel->outer_pixel = 1;
                    increased = 1;
                }
            }
        }
    }


    return increased;
}

/*!
 * \brief glyph_filler_find_new_inner_areas_secondary
 *
 * Find secondary inner area (in case the is crossing line)
 *
 * \param x
 * \param y
 * \param drawing
 * \param distance
 * \return
 */
static uint8_t glyph_filler_find_new_inner_areas_secondary(int x, int y, font_drawing_t *drawing, int distance)
{
    if (x <= 0 || y <= 0 || x >= drawing->width-1 || y >= drawing->height-1) {
        return 0;
    }

    uint8_t ret = 0;
    pixel_drawing_t *pixel_to;
    int x_plus, y_plus;
    for (x_plus=-1;x_plus<2;x_plus++) {
        for (y_plus=-1;y_plus<2;y_plus++) {
            if (x_plus != 0 || y_plus != 0) {
                pixel_to = glyph_filler_get_pixel(drawing, x+x_plus,  y+y_plus);
                if (pixel_to->line) {
                    if (distance > 0) {
                        if (glyph_filler_find_new_inner_areas_secondary(x+x_plus,  y+y_plus, drawing, distance-1)) {
                            ret = 1;
                        }
                    }
                } else if (!pixel_to->outer_pixel && !pixel_to->inner_pixel) {
                    if (x+x_plus >= drawing->line_min_x && y+y_plus >= drawing->line_min_y
                            && x+x_plus < drawing->line_max_x && y+y_plus < drawing->line_max_y) {
                        pixel_to->inner_pixel = 1;
                        ret = 1;
                    }
                }
            }
        }
    }

    return ret;
}

/*!
 * \brief glyph_filler_find_new_inner_areas
 *
 * find new inner areas
 *
 * \param drawing
 * \return
 */
static uint8_t glyph_filler_find_new_inner_areas(font_drawing_t *drawing)
{
    pixel_drawing_t *pixel;
    pixel_drawing_t *pixel_to;
    int x, y;
    int x_plus, y_plus;
    uint8_t is_outer_area;
    uint8_t is_inner_area;
    uint8_t ret = 0;
    for (y=drawing->line_min_y;y<drawing->line_max_y;y++) {
        for (x=drawing->line_min_x;x<drawing->line_max_x;x++) {
            pixel = glyph_filler_get_pixel(drawing, x,  y);
            if (pixel->line && !pixel->line_completed) {
                is_outer_area = 0;
                is_inner_area = 0;

                // check that if there is any "outer area or inner area"
                // next to line
                for (x_plus=-1;x_plus<2 && is_outer_area == 0;x_plus++) {
                    for (y_plus=-1;y_plus<2 && is_outer_area == 0;y_plus++) {
                        if (x_plus != 0 || y_plus != 0) {
                            if (x+x_plus >= 0 && y+y_plus >= 0
                                    && x+x_plus < drawing->width && y+y_plus < drawing->height) {
                                pixel_to = glyph_filler_get_pixel(drawing, x+x_plus,  y+y_plus);
                                if (pixel_to->outer_pixel) {
                                    is_outer_area = 1;
                                }
                                if (pixel_to->inner_pixel) {
                                    is_inner_area = 1;
                                }
                            }
                        }
                    }
                }

                if (is_outer_area) {
                    pixel->line_completed = 1;
                    for (x_plus=-1;x_plus<2;x_plus++) {
                        for (y_plus=-1;y_plus<2;y_plus++) {
                            if (x_plus != 0 || y_plus != 0) {
                                if (x+x_plus >= 0 && y+y_plus >= 0
                                        && x+x_plus >= drawing->line_min_x && y+y_plus >= drawing->line_min_y
                                        && x+x_plus < drawing->width && y+y_plus < drawing->height
                                        && x+x_plus < drawing->line_max_x && y+y_plus < drawing->line_max_y) {
                                    pixel_to = glyph_filler_get_pixel(drawing, x+x_plus,  y+y_plus);
                                    if (pixel_to->line) {
                                        if (pixel_to->line > 1 && !pixel_to->line_completed) {
                                            pixel_to->line_completed = 1;
                                            if (glyph_filler_find_new_inner_areas_secondary(x+x_plus, y+y_plus, drawing, 1)) {
                                                ret = 1;
                                            }
                                        }
                                    } else if (!pixel_to->outer_pixel && !pixel_to->line && !pixel_to->inner_pixel) {
                                        pixel_to->inner_pixel = 1;
                                        ret = 1;
                                    }
                                }
                            }
                        }
                    }
                } else if (is_inner_area) {
                    // there was an inner area near by, so it's completed
                    pixel->line_completed = 1;
                }
            }
        }
    }

    return ret;
}

/*!
 * \brief glyph_filler_find_new_outer_areas
 *
 * Find new outer area
 *
 * \param drawing
 * \return
 */
static uint8_t glyph_filler_find_new_outer_areas(font_drawing_t *drawing)
{
    pixel_drawing_t *pixel;
    pixel_drawing_t *pixel_to;
    int x, y;
    int x_plus, y_plus;
    uint8_t is_inner_area;
    uint8_t ret = 0;
    for (y=drawing->line_min_y+2;y<drawing->line_max_y-2;y++) {
        for (x=drawing->line_min_x+2;x<drawing->line_max_x-2;x++) {
            pixel = glyph_filler_get_pixel(drawing, x,  y);
            if (pixel->line && !pixel->line_completed) {
                is_inner_area = 0;

                for (x_plus=-1;x_plus<2 && is_inner_area == 0;x_plus++) {
                    for (y_plus=-1;y_plus<2 && is_inner_area == 0;y_plus++) {
                        if (x_plus != 0 || y_plus != 0) {
                            pixel_to = glyph_filler_get_pixel(drawing, x+x_plus,  y+y_plus);
                            if (pixel_to->inner_pixel) {
                                is_inner_area = 1;
                            }
                        }
                    }
                }

                if (is_inner_area) {
                    pixel->line_completed = 1;
                    for (x_plus=-1;x_plus<2;x_plus++) {
                        for (y_plus=-1;y_plus<2;y_plus++) {
                            if (x_plus != 0 || y_plus != 0) {
                                pixel_to = glyph_filler_get_pixel(drawing, x+x_plus,  y+y_plus);
                                if (!pixel_to->outer_pixel && !pixel_to->line && !pixel_to->inner_pixel) {
                                    pixel_to->outer_pixel = 1;
                                    ret = 1;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return ret;
}

#ifdef TIME_DEBUG
/*!
 * \brief get_time_difference
 *
 * Get time difference in seconds between start and end time
 *
 * \param time_start
 * \param time_end
 * \return time difference in seconds
 */
static double get_time_difference(const struct timespec *time_start, const struct timespec *time_end)
{
    double end_time = (double)time_end->tv_sec + (double)(time_end->tv_nsec)*0.000000001;
    double start_time = (double)time_start->tv_sec + (double)(time_start->tv_nsec)*0.000000001;
    return end_time - start_time;
}

static double time_glyph_filler_enlarge_outer_area = 0;
static double time_glyph_filler_find_new_inner_areas = 0;
static double time_glyph_filler_enlarge_inner_area = 0;
static double time_glyph_filler_find_new_outer_areas = 0;

/*!
 * \brief print_glyph_filler_times
 *
 * Print times
 *
 */
void print_glyph_filler_times(void)
{
    printf("Drawfont function times:\n");
    printf("glyph_filler_enlarge_outer_area: %f\n", time_glyph_filler_enlarge_outer_area);
    printf("glyph_filler_find_new_inner_areas: %f\n", time_glyph_filler_find_new_inner_areas);
    printf("glyph_filler_enlarge_inner_area: %f\n", time_glyph_filler_enlarge_inner_area);
    printf("glyph_filler_find_new_outer_areas: %f\n", time_glyph_filler_find_new_outer_areas);
}
#endif // TIME_DEBUG

/*!
 * \brief glyph_filler_draw_inner_area
 *
 * Set inner areas of the font
 *
 * \param drawing
 * \return
 */
int glyph_filler_draw_inner_area(font_drawing_t *drawing)
{
    uint8_t found = 1;
#ifdef TIME_DEBUG
    struct timespec time_start={0,0}, time_end={0,0};
    clock_gettime(CLOCK_MONOTONIC, &time_start);
#endif // TIME_DEBUG
    glyph_filler_enlarge_outer_area_first(drawing);
#ifdef TIME_DEBUG
        clock_gettime(CLOCK_MONOTONIC, &time_end);
        time_glyph_filler_enlarge_outer_area += get_time_difference(&time_start, &time_end);
#endif // TIME_DEBUG
    while (1) {
#ifdef TIME_DEBUG
        clock_gettime(CLOCK_MONOTONIC, &time_start);
#endif // TIME_DEBUG
        while (1) {
            if (!glyph_filler_enlarge_outer_area(drawing)) {
                break;
            } else {
                found = 1;
            }
        }

#ifdef TIME_DEBUG
        clock_gettime(CLOCK_MONOTONIC, &time_end);
        time_glyph_filler_enlarge_outer_area += get_time_difference(&time_start, &time_end);
#endif // TIME_DEBUG

        if (found) {
#ifdef TIME_DEBUG
            clock_gettime(CLOCK_MONOTONIC, &time_start);
#endif // TIME_DEBUG
            if (!glyph_filler_find_new_inner_areas(drawing)) {
                break;
            }
#ifdef TIME_DEBUG
            clock_gettime(CLOCK_MONOTONIC, &time_end);
            time_glyph_filler_find_new_inner_areas += get_time_difference(&time_start, &time_end);
            clock_gettime(CLOCK_MONOTONIC, &time_start);
#endif // TIME_DEBUG
            found = 0;
            while (1) {
                if (!glyph_filler_enlarge_inner_area(drawing)) {
                    break;
                }
                found = 1;
            }

#ifdef TIME_DEBUG
            clock_gettime(CLOCK_MONOTONIC, &time_end);
            time_glyph_filler_enlarge_inner_area += get_time_difference(&time_start, &time_end);
            clock_gettime(CLOCK_MONOTONIC, &time_start);
#endif // TIME_DEBUG
            if (found) {
                found = glyph_filler_find_new_outer_areas(drawing);
            }
#ifdef TIME_DEBUG
            clock_gettime(CLOCK_MONOTONIC, &time_end);
            time_glyph_filler_find_new_outer_areas += get_time_difference(&time_start, &time_end);
#endif // TIME_DEBUG
        }

        if (!found) {
            break;
        }

        found = 0;
    }

    return 0;
}
