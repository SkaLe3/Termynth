#pragma once

#include <cmath>
#include <cstdint>

inline float InvSqrt(float x)
{
    const float xhalf = 0.5f * x;

    int32_t i;
    i = * (int32_t *) &x;
    i = 0x5f3759df - (i >> 1); 

    float y = * (float *) &i;
    y = y * (1.5f - (xhalf * y * y));
    
    return y;
}