/*!
 * \file
 * \brief file matrix4x4.cpp
 *
 * Simple 4x4 matrix
 *
 * Copyright of Timo Hannukkala. All rights reserved.
 *
 * \author Timo Hannukkala <timo.hannukkala@hotmail.com>
 */

#include "matrix4x4.h"
#include <string.h>

/*!
 * \brief Matrix4x4::Matrix4x4
 *
 * Constructor
 */
Matrix4x4::Matrix4x4()
{
    memset(&m_matrixTable, 0, sizeof(Matrix4x4Table));
    m_matrixTable.matrix4x4Values.m00 = 1.0;
    m_matrixTable.matrix4x4Values.m11 = 1.0;
    m_matrixTable.matrix4x4Values.m22 = 1.0;
    m_matrixTable.matrix4x4Values.m33 = 1.0;
}

/*!
 * \brief Matrix4x4::getMatrixTable
 * \return matrix table
 */
Matrix4x4Table *Matrix4x4::getMatrixTable()
{
    return &m_matrixTable;
}


/*!
 * \brief Matrix4x4::setOrth
 *
 * set orthographic matrix
 *
 * \param left
 * \param right
 * \param bottom
 * \param top
 * \param znear
 * \param zfar
 */
void Matrix4x4::setOrth(float left, float right, float top, float bottom, float znear, float zfar)
{
    memset(&m_matrixTable, 0, sizeof(Matrix4x4Table));

    m_matrixTable.matrix4x4Values.m00 = +2.0f/(right-left);
    m_matrixTable.matrix4x4Values.m30 = -(right+left)/(right-left);
    m_matrixTable.matrix4x4Values.m11 = +2.0f/(top-bottom);
    m_matrixTable.matrix4x4Values.m31 = -(top+bottom)/(top-bottom);
    m_matrixTable.matrix4x4Values.m22 = -2.0f/(zfar-znear);
    m_matrixTable.matrix4x4Values.m32 = -(zfar+znear)/(zfar-znear);
    m_matrixTable.matrix4x4Values.m33 = 1.0f;
}
