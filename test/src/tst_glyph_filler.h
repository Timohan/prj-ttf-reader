/*!
* \file
* \brief file tst_glyph_filler.h
*
* glyph_filler unit tests
*
* Copyright of Timo Hannukkala, All rights reserved.
*
* \author Timo Hannukkala <timohannukkala@hotmail.com>
*/

#ifndef TST_GLYPHFILLER_H
#define TST_GLYPHFILLER_H

#include <stddef.h>
#include <vector>

class TstFillerPoint {
public:
    TstFillerPoint(size_t x, size_t y);

    size_t m_x;
    size_t m_y;

    static bool isFillerPointInListCorrectly(size_t x, size_t y,
                                             std::vector<TstFillerPoint> &listCorrectPoints, bool innerpixel_drawing);
};


#endif // TST_GLYPHFILLER_H
