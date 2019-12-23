/*!
* \file
* \brief file tst_test_loader.cpp
*
* tst_test_loader unit tests
*
* Copyright of Timo Hannukkala, All rights reserved.
*
* \author Timo Hannukkala <timohannukkala@hotmail.com>
*/
#include "tst_test_loader.h"
#include <stdio.h>
#include "tst_glyph_filler.h"
#include "test_loader.h"

/*!
 * \brief tst_test_loader_rotate
 *
 * Basic testing that rotation on test_loader goes correctly
 *
 * \return
 */
int tst_test_loader_rotate()
{
    size_t i;
    size_t xOut, yOut;
    std::vector<TstFillerPoint>listFillerPoint;
    std::vector<TstFillerPoint>listFillerPointResult;

    listFillerPoint.push_back(TstFillerPoint(0, 0));
    listFillerPointResult.push_back(TstFillerPoint(4, 0));
    listFillerPoint.push_back(TstFillerPoint(4, 0));
    listFillerPointResult.push_back(TstFillerPoint(4, 4));
    listFillerPoint.push_back(TstFillerPoint(4, 4));
    listFillerPointResult.push_back(TstFillerPoint(0, 4));
    listFillerPoint.push_back(TstFillerPoint(0, 4));
    listFillerPointResult.push_back(TstFillerPoint(0, 0));

    listFillerPoint.push_back(TstFillerPoint(1, 1));
    listFillerPointResult.push_back(TstFillerPoint(3, 1));
    listFillerPoint.push_back(TstFillerPoint(3, 1));
    listFillerPointResult.push_back(TstFillerPoint(3, 3));
    listFillerPoint.push_back(TstFillerPoint(3, 3));
    listFillerPointResult.push_back(TstFillerPoint(1, 3));
    listFillerPoint.push_back(TstFillerPoint(1, 3));
    listFillerPointResult.push_back(TstFillerPoint(1, 1));

    listFillerPoint.push_back(TstFillerPoint(2, 2));
    listFillerPointResult.push_back(TstFillerPoint(2, 2));

    for (i=0;i<listFillerPoint.size();i++) {
        tst_rotate(listFillerPoint.at(i).m_x, listFillerPoint.at(i).m_y, &xOut, &yOut, 5, 5, LoadTestImageRotation::LoadTestImageRotation90ClockWise);
        if (xOut != listFillerPointResult.at(i).m_x || yOut != listFillerPointResult.at(i).m_y) {
            return 1;
        }
    }

    listFillerPoint.clear();
    listFillerPointResult.clear();
    listFillerPoint.push_back(TstFillerPoint(0, 0));
    listFillerPointResult.push_back(TstFillerPoint(0, 4));
    listFillerPoint.push_back(TstFillerPoint(0, 4));
    listFillerPointResult.push_back(TstFillerPoint(4, 4));
    listFillerPoint.push_back(TstFillerPoint(4, 4));
    listFillerPointResult.push_back(TstFillerPoint(4, 0));
    listFillerPoint.push_back(TstFillerPoint(4, 0));
    listFillerPointResult.push_back(TstFillerPoint(0, 0));

    listFillerPoint.push_back(TstFillerPoint(1, 1));
    listFillerPointResult.push_back(TstFillerPoint(1, 3));
    listFillerPoint.push_back(TstFillerPoint(1, 3));
    listFillerPointResult.push_back(TstFillerPoint(3, 3));
    listFillerPoint.push_back(TstFillerPoint(3, 3));
    listFillerPointResult.push_back(TstFillerPoint(3, 1));
    listFillerPoint.push_back(TstFillerPoint(3, 1));
    listFillerPointResult.push_back(TstFillerPoint(1, 1));

    for (i=0;i<listFillerPoint.size();i++) {
        tst_rotate(listFillerPoint.at(i).m_x, listFillerPoint.at(i).m_y, &xOut, &yOut, 5, 5, LoadTestImageRotation::LoadTestImageRotation90CounterClockWise);
        if (xOut != listFillerPointResult.at(i).m_x || yOut != listFillerPointResult.at(i).m_y) {
            return 2;
        }
    }

    return 0;
}
