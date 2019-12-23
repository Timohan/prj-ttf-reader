/*!
 * \file
 * \brief file matrix4x4.h
 *
 * Simple 4x4 matrix
 *
 * Copyright of Timo Hannukkala. All rights reserved.
 *
 * \author Timo Hannukkala <timo.hannukkala@hotmail.com>
 */

#ifndef MATRIX4X4_H
#define MATRIX4X4_H

typedef union
{
    float data[16];    // to get all matrix values
    struct {
        float m00, m01, m02, m03;
        float m10, m11, m12, m13;
        float m20, m21, m22, m23;
        float m30, m31, m32, m33;
    } matrix4x4Values;
} Matrix4x4Table;

class Matrix4x4
{
public:
    Matrix4x4();
    Matrix4x4Table *getMatrixTable();

    void setOrth(float left, float right, float top, float bottom, float znear, float zfar);
private:
    Matrix4x4Table m_matrixTable;
};

#endif // MATRIX4X4_H
