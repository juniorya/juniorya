/**
 * @file q16.c
 * @brief Implementation of fixed-point Q16.16 arithmetic primitives.
 */

#include "q16.h"

#include <limits.h>
#include <stddef.h>

#define Q16_FRACTIONAL_BITS 16
#define Q16_ONE (1 << Q16_FRACTIONAL_BITS)

static q16_16 saturate_int64(int64_t value)
{
    if (value > INT32_MAX)
    {
        return INT32_MAX;
    }
    if (value < INT32_MIN)
    {
        return INT32_MIN;
    }
    return (q16_16)value;
}

q16_16 q16_from_int(int32_t value)
{
    return (q16_16)((int64_t)value << Q16_FRACTIONAL_BITS);
}

q16_16 q16_from_float(float value)
{
    const float scaled = value * (float)Q16_ONE;
    if (scaled > (float)INT32_MAX)
    {
        return INT32_MAX;
    }
    if (scaled < (float)INT32_MIN)
    {
        return INT32_MIN;
    }
    return (q16_16)(scaled >= 0.0f ? scaled + 0.5f : scaled - 0.5f);
}

float q16_to_float(q16_16 value)
{
    return (float)value / (float)Q16_ONE;
}

int32_t q16_to_int(q16_16 value)
{
    if (value >= 0)
    {
        return (int32_t)(value >> Q16_FRACTIONAL_BITS);
    }
    return - (int32_t)((-value + (Q16_ONE - 1)) >> Q16_FRACTIONAL_BITS);
}

q16_16 q16_add(q16_16 a, q16_16 b)
{
    int64_t result = (int64_t)a + (int64_t)b;
    return saturate_int64(result);
}

q16_16 q16_sub(q16_16 a, q16_16 b)
{
    int64_t result = (int64_t)a - (int64_t)b;
    return saturate_int64(result);
}

q16_16 q16_mul(q16_16 a, q16_16 b)
{
    int64_t product = (int64_t)a * (int64_t)b;
    product += (int64_t)1 << (Q16_FRACTIONAL_BITS - 1);
    product >>= Q16_FRACTIONAL_BITS;
    return saturate_int64(product);
}

q16_16 q16_div(q16_16 numerator, q16_16 denominator, bool *ok)
{
    if (denominator == 0)
    {
        if (ok != NULL)
        {
            *ok = false;
        }
        return 0;
    }
    int64_t dividend = ((int64_t)numerator << Q16_FRACTIONAL_BITS);
    int64_t result = dividend / (int64_t)denominator;
    if (ok != NULL)
    {
        *ok = true;
    }
    return saturate_int64(result);
}

q16_16 q16_sqrt(q16_16 value)
{
    if (value <= 0)
    {
        return 0;
    }
    int64_t x = (int64_t)value;
    int64_t guess = x;
    for (int i = 0; i < 8; ++i)
    {
        guess = (guess + x * Q16_ONE / guess) / 2;
    }
    return saturate_int64(guess);
}

q16_16 q16_abs(q16_16 value)
{
    if (value < 0)
    {
        return (q16_16)(-value);
    }
    return value;
}
