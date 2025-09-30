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
int32_t decrease_min_value(int32_t value, int32_t quality, const float value_to_correct);
int32_t increase_max_value(int32_t value, int32_t quality, const float value_to_correct);

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

    if (decrease_min_value(3, 5, 5) != 3) {
        return 11;
    }
    if (decrease_min_value(3, 5, 0) != -2) {
        return 12;
    }
    if (decrease_min_value(3, 5, -1) != -2) {
        return 13;
    }
    if (decrease_min_value(3, 5, -2) != -7) {
        return 14;
    }
    if (decrease_min_value(3, 5, -6) != -7) {
        return 15;
    }
    if (decrease_min_value(3, 5, -7) != -12) {
        return 16;
    }
    if (decrease_min_value(3, 5, 3) != -2) {
        return 17;
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

    if (increase_max_value(3, 5, 0) != 3) {
        return 14;
    }
    if (increase_max_value(3, 5, 2) != 3) {
        return 15;
    }
    if (increase_max_value(3, 5, 3) != 8) {
        return 16;
    }
    if (increase_max_value(3, 5, 4) != 8) {
        return 17;
    }
    if (increase_max_value(3, 5, 7) != 8) {
        return 18;
    }
    if (increase_max_value(3, 5, 8) != 13) {
        return 19;
    }
    if (increase_max_value(3, 5, 9) != 13) {
        return 20;
    }
    if (increase_max_value(3, 5, 13) != 18) {
        return 21;
    }
    if (increase_max_value(3, 5, 14) != 18) {
        return 22;
    }
    return 0;
}
