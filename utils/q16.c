#include "utils/q16.h"

#include <math.h>

#define Q16_SHIFT 16
#define Q16_ONE   (1 << Q16_SHIFT)

q16_16 q16_from_int(int32_t value)
{
    return (q16_16)(value << Q16_SHIFT);
}

q16_16 q16_from_float(float value)
{
    return (q16_16)lrintf(value * (float)Q16_ONE);
}

float q16_to_float(q16_16 value)
{
    return (float)value / (float)Q16_ONE;
}

q16_16 q16_mul(q16_16 a, q16_16 b)
{
    int64_t prod = (int64_t)a * (int64_t)b;
    prod >>= Q16_SHIFT;
    if (prod > INT32_MAX)
    {
        return INT32_MAX;
    }
    if (prod < INT32_MIN)
    {
        return INT32_MIN;
    }
    return (q16_16)prod;
}

q16_16 q16_div(q16_16 a, q16_16 b)
{
    if (b == 0)
    {
        return (a >= 0) ? INT32_MAX : INT32_MIN;
    }
    int64_t num = ((int64_t)a << Q16_SHIFT);
    int64_t res = num / b;
    if (res > INT32_MAX)
    {
        return INT32_MAX;
    }
    if (res < INT32_MIN)
    {
        return INT32_MIN;
    }
    return (q16_16)res;
}

q16_16 q16_sqrt(q16_16 value)
{
    if (value <= 0)
    {
        return 0;
    }
    double fp = sqrt((double)value / (double)Q16_ONE);
    return q16_from_float((float)fp);
}

q16_16 q16_sin(q16_16 radians)
{
    double fp = sin((double)radians / (double)Q16_ONE);
    return q16_from_float((float)fp);
}

q16_16 q16_cos(q16_16 radians)
{
    double fp = cos((double)radians / (double)Q16_ONE);
    return q16_from_float((float)fp);
}

q16_16 q16_clamp(q16_16 value, q16_16 min, q16_16 max)
{
    if (value < min)
    {
        return min;
    }
    if (value > max)
    {
        return max;
    }
    return value;
}

q16_16 q16_abs(q16_16 value)
{
    return (value < 0) ? (q16_16)(-value) : value;
}
