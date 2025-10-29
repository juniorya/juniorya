#ifndef UTILS_FIXED_H
#define UTILS_FIXED_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int32_t q16_16_t;

#define Q16_16_ONE ((q16_16_t)0x00010000)
#define Q16_16_HALF ((q16_16_t)0x00008000)
#define Q16_16_PI ((q16_16_t)205887) /* 3.1415926 * 2^16 */

static inline q16_16_t q16_16_from_int(int32_t value)
{
    return value << 16;
}

static inline int32_t q16_16_to_int(q16_16_t value)
{
    return value >> 16;
}

static inline q16_16_t q16_16_from_float(float value)
{
    return (q16_16_t)(value * 65536.0f);
}

static inline float q16_16_to_float(q16_16_t value)
{
    return (float)value / 65536.0f;
}

static inline q16_16_t q16_16_mul(q16_16_t a, q16_16_t b)
{
    int64_t temp = (int64_t)a * (int64_t)b;
    temp += Q16_16_HALF;
    return (q16_16_t)(temp >> 16);
}

static inline q16_16_t q16_16_div(q16_16_t a, q16_16_t b)
{
    int64_t temp = ((int64_t)a << 16);
    if (b == 0) {
        return 0;
    }
    temp += (b > 0 ? b / 2 : -b / 2);
    return (q16_16_t)(temp / b);
}

static inline q16_16_t q16_16_clamp(q16_16_t value, q16_16_t min, q16_16_t max)
{
    if (value < min) {
        return min;
    }
    if (value > max) {
        return max;
    }
    return value;
}

static inline q16_16_t q16_16_abs(q16_16_t value)
{
    return value < 0 ? -value : value;
}

q16_16_t q16_16_sqrt(q16_16_t value);

#ifdef __cplusplus
}
#endif

#endif
