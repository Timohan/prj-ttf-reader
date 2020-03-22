/*!
* \file
* \brief file tst_glyph_graph_generator.cpp
*
* glyph_graph_generator unit tests
*
* Copyright of Timo Hannukkala, All rights reserved.
*
* \author Timo Hannukkala <timohannukkala@hotmail.com>
*/
#include "tst_glyph_graph_generator.h"
#include <stdio.h>
#include <stdint.h>

int32_t get_min_value(float value, int quality);
int32_t get_max_value(float value, int quality);
int32_t get_zero_line_value(float value, int quality);

/*!
 * \brief tst_glyph_graph_generator_min_value
 *
 * Tests get_min_value in glyph_graph_generator
 *
 * \return 0 on success
 */
int tst_glyph_graph_generator_min_value()
{
    float value;

    if (get_min_value(0.0f, 5) != -5) {
        return 1;
    }

    for (value=-0.01f;value>=-4.9f;value-=0.01f) {
        if (get_min_value(value, 5) != -10) {
            return 2;
        }
    }

    if (get_min_value(-5.0f, 5) != -10) {
        return 3;
    }

    for (value=-5.01f;value>=-9.9f;value-=0.01f) {
        if (get_min_value(value, 5) != -15) {
            return 4;
        }
    }

    if (get_min_value(-10.0f, 5) != -15) {
        return 5;
    }

    for (value=0.01f;value<=4.9f;value+=0.01f) {
        if (get_min_value(value, 5) != -5) {
            return 6;
        }
    }

    if (get_min_value(5.0f, 5) != 0) {
        return 7;
    }

    for (value=5.01f;value<=9.9f;value+=0.01f) {
        if (get_min_value(value, 5) != 0) {
            return 8;
        }
    }

    if (get_min_value(10.0f, 5) != 5) {
        return 9;
    }

    for (value=10.01f;value<=14.9f;value+=0.01f) {
        if (get_min_value(value, 5) != 5) {
            return 10;
        }
    }

    if (get_min_value(0.0f, 10) != -10) {
        return 1;
    }

    for (value=-0.01f;value>=-9.9f;value-=0.01f) {
        if (get_min_value(value, 10) != -20) {
            return 2;
        }
    }

    return 0;
}
/*!
 * \brief tst_glyph_graph_generator_max_value
 *
 * Tests get_max_value in glyph_graph_generator
 *
 * \return 0 on success
 */
int tst_glyph_graph_generator_max_value()
{
    float value;

    if (get_max_value(0.0f, 5) != 5) {
        return 1;
    }

    for (value=0.01f;value<4.9f;value+=0.01f) {
        if (get_max_value(value, 5) != 10) {
            return 2;
        }
    }

    if (get_max_value(5.0f, 5) != 10) {
        return 3;
    }

    for (value=5.01f;value<=9.9f;value+=0.01f) {
        if (get_max_value(value, 5) != 15) {
            return 4;
        }
    }

    if (get_max_value(10.0f, 5) != 15) {
        return 5;
    }

    for (value=-0.01f;value>=-4.9f;value-=0.01f) {
        if (get_max_value(value, 5) != 5) {
            return 6;
        }
    }

    if (get_max_value(-5.0f, 5) != 0) {
        return 7;
    }

    for (value=-5.01f;value>=-9.9f;value-=0.01f) {
        if (get_max_value(value, 5) != 0) {
            return 8;
        }
    }

    if (get_max_value(-10.0f, 5) != -5) {
        return 9;
    }

    for (value=-10.01f;value>=-14.9f;value-=0.01f) {
        if (get_max_value(value, 5) != -5) {
            return 10;
        }
    }

    if (get_max_value(0.0f, 10) != 10) {
        return 11;
    }

    for (value=0.01f;value<9.9f;value+=0.01f) {
        if (get_max_value(value, 10) != 20) {
            return 12;
        }
    }

    if (get_max_value(10.0f, 10) != 20) {
        return 13;
    }

    return 0;
}

/*!
 * \brief tst_glyph_graph_generator_zero_line_value
 *
 * test get_zero_line_value
 * \return 0 on success
 */
int tst_glyph_graph_generator_zero_line_value()
{
    float value;

    if (get_zero_line_value(0.0f, 5) != 0) {
        return 1;
    }

    for (value=0.01f;value<4.9f;value+=0.01f) {
        if (get_zero_line_value(value, 5) != 0) {
            return 2;
        }
    }

    if (get_zero_line_value(5.0f, 5) != 1) {
        return 2;
    }

    for (value=5.01f;value<9.9f;value+=0.01f) {
        if (get_zero_line_value(value, 5) != 1) {
            return 3;
        }
    }

    if (get_zero_line_value(10.0f, 5) != 2) {
        return 4;
    }

    for (value=10.01f;value<14.9f;value+=0.01f) {
        if (get_zero_line_value(value, 5) != 2) {
            return 5;
        }
    }

    if (get_zero_line_value(15.0f, 5) != 3) {
        return 6;
    }

    for (value=-0.01f;value>=-4.9f;value-=0.01f) {
        if (get_zero_line_value(value, 5) != -1) {
            return 7;
        }
    }

    if (get_zero_line_value(-5.0f, 5) != -1) {
        return 8;
    }

    for (value=-5.01f;value>=-9.9f;value-=0.01f) {
        if (get_zero_line_value(value, 5) != -2) {
            return 9;
        }
    }

    if (get_zero_line_value(-10.0f, 5) != -2) {
        printf("%d\n", get_zero_line_value(-5.0f, 5));
        return 10;
    }



    if (get_zero_line_value(0.0f, 10) != 0) {
        return 11;
    }

    for (value=0.01f;value<9.9f;value+=0.01f) {
        if (get_zero_line_value(value, 10) != 0) {
            return 12;
        }
    }

    if (get_zero_line_value(10.0f, 10) != 1) {
        return 13;
    }

    for (value=10.01f;value<19.9f;value+=0.01f) {
        if (get_zero_line_value(value, 10) != 1) {
            return 14;
        }
    }

    if (get_zero_line_value(20.0f, 10) != 2) {
        return 15;
    }

    return 0;
}
