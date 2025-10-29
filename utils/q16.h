/**
 * @file q16.h
 * @brief Fixed-point Q16.16 arithmetic utilities used across the CNC stack.
 *
 * This module implements saturated arithmetic helpers and conversion routines for
 * the Q16.16 fixed-point format. The format stores 16 integer bits and 16
 * fractional bits and is used for deterministic execution on targets that lack
 * floating-point support in the real-time path. All functions are implemented as
 * static inline definitions to keep the overhead minimal when invoked from
 * higher-level control loops.
 */
#ifndef UTILS_Q16_H
#define UTILS_Q16_H

#include <stdint.h>
#include <stdbool.h>

/** @brief Q16.16 number represented as a signed 32-bit integer. */
typedef int32_t q16_16;

/** @brief Scaling factor between integer and fractional representations. */
#define Q16_16_ONE ((q16_16)0x00010000)

/**
 * @brief Convert a signed integer to Q16.16.
 * @param value Integer value to convert.
 * @return Q16.16 representation.
 */
static inline q16_16 q16_from_int(int32_t value)
{
    return (q16_16)(value << 16);
}

/**
 * @brief Convert a Q16.16 value to integer using truncation.
 * @param value Q16.16 value to convert.
 * @return Integer representation with truncated fractional part.
 */
static inline int32_t q16_to_int(q16_16 value)
{
    return (int32_t)(value >> 16);
}

/**
 * @brief Convert a floating-point value to Q16.16 with saturation.
 * @param value Floating-point value to convert.
 * @return Saturated Q16.16 representation.
 */
static inline q16_16 q16_from_float(float value)
{
    const float scaled = value * 65536.0f;
    if (scaled > 2147483647.0f)
    {
        return (q16_16)2147483647;
    }
    if (scaled < -2147483648.0f)
    {
        return (q16_16)-2147483648;
    }
    return (q16_16)scaled;
}

/**
 * @brief Convert a Q16.16 value to floating-point.
 * @param value Q16.16 value to convert.
 * @return Floating-point representation.
 */
static inline float q16_to_float(q16_16 value)
{
    return (float)value / 65536.0f;
}

/**
 * @brief Saturating addition.
 * @param a Q16.16 augend.
 * @param b Q16.16 addend.
 * @return Saturated sum of the inputs.
 */
static inline q16_16 q16_add(q16_16 a, q16_16 b)
{
    int64_t sum = (int64_t)a + (int64_t)b;
    if (sum > 2147483647)
    {
        sum = 2147483647;
    }
    else if (sum < -2147483648)
    {
        sum = -2147483648;
    }
    return (q16_16)sum;
}

/**
 * @brief Saturating subtraction.
 * @param a Minuend.
 * @param b Subtrahend.
 * @return Saturated difference of the inputs.
 */
static inline q16_16 q16_sub(q16_16 a, q16_16 b)
{
    int64_t diff = (int64_t)a - (int64_t)b;
    if (diff > 2147483647)
    {
        diff = 2147483647;
    }
    else if (diff < -2147483648)
    {
        diff = -2147483648;
    }
    return (q16_16)diff;
}

/**
 * @brief Saturating multiplication.
 * @param a Multiplicand.
 * @param b Multiplier.
 * @return Saturated product of the inputs.
 */
static inline q16_16 q16_mul(q16_16 a, q16_16 b)
{
    int64_t prod = (int64_t)a * (int64_t)b;
    prod >>= 16;
    if (prod > 2147483647)
    {
        prod = 2147483647;
    }
    else if (prod < -2147483648)
    {
        prod = -2147483648;
    }
    return (q16_16)prod;
}

/**
 * @brief Saturating division with zero guard.
 * @param a Dividend.
 * @param b Divisor.
 * @return Saturated quotient, zero if divisor is zero.
 */
static inline q16_16 q16_div(q16_16 a, q16_16 b)
{
    if (b == 0)
    {
        return 0;
    }
    int64_t dividend = ((int64_t)a << 16);
    int64_t quotient = dividend / b;
    if (quotient > 2147483647)
    {
        quotient = 2147483647;
    }
    else if (quotient < -2147483648)
    {
        quotient = -2147483648;
    }
    return (q16_16)quotient;
}

/**
 * @brief Clamp a value between two bounds.
 * @param value Input value.
 * @param min_value Minimum allowed value.
 * @param max_value Maximum allowed value.
 * @return Clamped value.
 */
static inline q16_16 q16_clamp(q16_16 value, q16_16 min_value, q16_16 max_value)
{
    if (value < min_value)
    {
        return min_value;
    }
    if (value > max_value)
    {
        return max_value;
    }
    return value;
}

#endif /* UTILS_Q16_H */
