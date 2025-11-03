#ifndef Q16_H
#define Q16_H

#include <stdint.h>
#include <stdbool.h>

/** \brief Fixed-point Q16.16 number representation. */
typedef int32_t q16_16;

/** \brief Convert integer to Q16.16. */
q16_16 q16_from_int(int32_t value);

/** \brief Convert floating point to Q16.16. */
q16_16 q16_from_float(float value);

/** \brief Convert Q16.16 to float. */
float q16_to_float(q16_16 value);

/** \brief Multiply two Q16.16 values. */
q16_16 q16_mul(q16_16 a, q16_16 b);

/** \brief Divide two Q16.16 values with saturation. */
q16_16 q16_div(q16_16 a, q16_16 b);

/** \brief Square root using Newton iterations. */
q16_16 q16_sqrt(q16_16 value);

/** \brief Sine of a Q16.16 angle expressed in radians. */
q16_16 q16_sin(q16_16 radians);

/** \brief Cosine of a Q16.16 angle expressed in radians. */
q16_16 q16_cos(q16_16 radians);

/** \brief Clamp value between limits. */
q16_16 q16_clamp(q16_16 value, q16_16 min, q16_16 max);

/** \brief Absolute value helper. */
q16_16 q16_abs(q16_16 value);

#endif
