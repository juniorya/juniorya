/**
 * @file lut_trig.h
 * @brief Lookup-table trig functions for deterministic execution.
 */

#ifndef LUT_TRIG_H
#define LUT_TRIG_H

#include "q16.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialise sine/cosine lookup table.
 */
void lut_trig_init(void);

/**
 * @brief Compute sine of angle in radians using lookup + interpolation.
 * @param angle_rad Angle in Q16.16 radians.
 * @return Sine in Q16.16.
 */
q16_16 lut_sin(q16_16 angle_rad);

/**
 * @brief Compute cosine of angle in radians using lookup + interpolation.
 * @param angle_rad Angle in Q16.16 radians.
 * @return Cosine in Q16.16.
 */
q16_16 lut_cos(q16_16 angle_rad);

#ifdef __cplusplus
}
#endif

#endif
