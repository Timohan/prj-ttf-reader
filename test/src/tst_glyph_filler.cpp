/*!
* \file
* \brief file tst_glyph_filler.cpp
*
* glyph_filler unit tests
*
* Copyright of Timo Hannukkala, All rights reserved.
*
* \author Timo Hannukkala <timohannukkala@hotmail.com>
*/

#include "tst_glyph_filler.h"
#include "../../lib/src/drawfont/glyph_filler.h"
#include "../../lib/src/drawfont/glyph_drawer.h"
/*!
 * \brief TstFillerPoint::TstFillerPoint
 * \param x
 * \param y
 */
TstFillerPoint::TstFillerPoint(size_t x, size_t y) :
    m_x(x), m_y(y)
{
}

/*!
 * \brief TstFillerPoint::isFillerPointInListCorrectly
 *
 * checks if the innerpixel_drawing (x/y) is in the listCorrectPoints
 *
 * \param x
 * \param y
 * \param listCorrectPoints
 * \param innerpixel_drawing if true, x/y needs to be in listCorrectPoints, if false, x/y must not be in listCorrectPoints
 * \return true if it's correctly (check innerpixel_drawing)
 */
bool TstFillerPoint::isFillerPointInListCorrectly(size_t x, size_t y,
                                                  std::vector<TstFillerPoint> &listCorrectPoints, bool innerpixel_drawing) {
    std::vector<TstFillerPoint>::iterator it;
    for (it=listCorrectPoints.begin();it!=listCorrectPoints.end();it++) {
        if (it->m_x == x && it->m_y == y) {
            return innerpixel_drawing;
        }
    }
    return !innerpixel_drawing;
}


