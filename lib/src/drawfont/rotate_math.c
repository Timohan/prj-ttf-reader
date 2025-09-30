#include "rotate_math.h"
#include <math.h>

static float fix_angle(float angle)
{
    while (1) {
        if (angle >= 0 && angle < (float)(M_PI*2)) {
            return angle;
        }

        if (angle < 0) {
            angle += (float)(M_PI*2);
        } else {
            angle -= (float)(M_PI*2);
        }
    }
}

static float calculate_angle(float x, float y)
{
    if (fabsf(x) <= 0) {
        if (y >= 0) {
            return 0;
        }
        return (float)M_PI;
    }

    if (fabsf(y) <= 0) {
        if (x >= 0) {
            return (float)(M_PI/2.0);
        }
        return (float)(M_PI + M_PI/2.0);
    }

    if (x >= 0 && y >= 0) {
        return atanf(x/y);
    }

    if (x >= 0 && y <= 0) {
        return (float)(M_PI) + atanf(x/y);
    }

    if (x <= 0 && y <= 0) {
        return (float)(M_PI) + atanf(x/y);
    }

    return (float)(M_PI*2) + atanf(x/y);
}

void rotate_by_angle_zero(float *out_x, float *out_y, const float x, const float y, float angle)
{
    if (angle <= 0) {
        *out_x = x;
        *out_y = y;
        return;
    }

    angle += calculate_angle(x, y);
    const float distance = sqrtf(x*x + y*y);
    angle = fix_angle(angle);

    *out_x = sinf(angle)*distance;
    *out_y = cosf(angle)*distance;
}

void rotate_by_angle(float *out_x, float *out_y, const float origin_x, const float origin_y, const float x, const float y, float angle)
{
    if (angle <= 0) {
        *out_x = x;
        *out_y = y;
        return;
    }

    float rotated_x, rotated_y;
    rotate_by_angle_zero(&rotated_x, &rotated_y, x - origin_x, y - origin_y, angle);
    *out_x = rotated_x + origin_x;
    *out_y = rotated_y + origin_y;
}