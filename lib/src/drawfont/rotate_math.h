/*!
 * \file
 * \brief file rotate_math.h
 *
 * rotate math
 *
 * Copyright of Timo Hannukkala. All rights reserved.
 *
 * \author Timo Hannukkala <timohannukkala@hotmail.com>
 */

#ifndef ROTATE_MATH_H
#define ROTATE_MATH_H

void rotate_by_angle_zero(float *out_x, float *out_y, const float x, const float y, float angle);
void rotate_by_angle(float *out_x, float *out_y, const float origin_x, const float origin_y, const float x, const float y, float angle);

#endif // ROTATE_MATH_H
