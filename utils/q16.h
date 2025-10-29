/**
 * @file q16.h
 * @brief Fixed-point Q16.16 arithmetic utilities.
 *
 * The planner and kinematics execute largely in fixed-point arithmetic to
 * ensure deterministic execution across all supported RTOS targets. The
 * Q16.16 format provides a range of ±32768 with ~15 µm resolution when units are
 * millimetres. Saturating helpers are provided for common operations.
 */

#ifndef Q16_H
#define Q16_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Q16.16 fixed-point type. */
typedef int32_t q16_16;

/**
 * @brief Convert integer to Q16.16.
 * @param value Integer value to convert.
 * @return Fixed-point representation.
 */
q16_16 q16_from_int(int32_t value);

/**
 * @brief Convert floating point number to Q16.16.
 * @param value Floating point number.
 * @return Fixed-point representation with rounding.
 */
q16_16 q16_from_float(float value);

/**
 * @brief Convert Q16.16 to floating point.
 * @param value Fixed-point value.
 * @return Floating point equivalent.
 */
float q16_to_float(q16_16 value);

/**
 * @brief Convert Q16.16 to integer using floor semantics.
 * @param value Fixed-point value.
 * @return Integer truncated towards negative infinity.
 */
int32_t q16_to_int(q16_16 value);

/**
 * @brief Saturating addition of two fixed-point numbers.
 * @param a First operand.
 * @param b Second operand.
 * @return Sum with saturation to Q16.16 limits.
 */
q16_16 q16_add(q16_16 a, q16_16 b);

/**
 * @brief Saturating subtraction of two fixed-point numbers.
 * @param a Minuend.
 * @param b Subtrahend.
 * @return Difference with saturation.
 */
q16_16 q16_sub(q16_16 a, q16_16 b);

/**
 * @brief Multiply two fixed-point values.
 * @param a First operand.
 * @param b Second operand.
 * @return Product scaled back to Q16.16.
 */
q16_16 q16_mul(q16_16 a, q16_16 b);

/**
 * @brief Divide two fixed-point values.
 * @param numerator Numerator in Q16.16.
 * @param denominator Denominator in Q16.16.
 * @param ok Optional status flag; set to false if division by zero occurs.
 * @return Quotient or zero on division by zero.
 */
q16_16 q16_div(q16_16 numerator, q16_16 denominator, bool *ok);

/**
 * @brief Compute square root using Newton iterations.
 * @param value Non-negative Q16.16 value.
 * @return Square root in Q16.16.
 */
q16_16 q16_sqrt(q16_16 value);

/**
 * @brief Compute absolute value.
 * @param value Input value.
 * @return Absolute value in Q16.16.
 */
q16_16 q16_abs(q16_16 value);

#ifdef __cplusplus
}
#endif

#endif
