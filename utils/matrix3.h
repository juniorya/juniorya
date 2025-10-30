/**
 * @file matrix3.h
 * @brief 3x3 matrix helpers for Jacobian computations.
 */

#ifndef MATRIX3_H
#define MATRIX3_H

#include "vec3.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Row-major 3x3 matrix in Q16.16 format. */
typedef struct
{
    q16_16 m[3][3]; /**< Matrix elements. */
} matrix3_q16;

/**
 * @brief Compute determinant of a 3x3 matrix.
 * @param mat Matrix to evaluate.
 * @return Determinant in Q16.16.
 */
q16_16 matrix3_det(const matrix3_q16 *mat);

/**
 * @brief Compute inverse of 3x3 matrix if invertible.
 * @param mat Matrix to invert.
 * @param inverse Output matrix; undefined when return value is false.
 * @return True if inverse exists.
 */
bool matrix3_inverse(const matrix3_q16 *mat, matrix3_q16 *inverse);

#ifdef __cplusplus
}
#endif

#endif
