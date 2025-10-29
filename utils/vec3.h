/**
 * @file vec3.h
 * @brief Three-dimensional vector helpers for Q16.16 arithmetic.
 *
 * The vector utilities provide basic algebra routines for delta kinematics and
 * trajectory generation. All routines operate on fixed-point values to maintain
 * deterministic runtime behavior across supported real-time operating systems.
 */
#ifndef UTILS_VEC3_H
#define UTILS_VEC3_H

#include "q16.h"

/**
 * @brief Q16.16 vector with three Cartesian components.
 */
typedef struct
{
    q16_16 x; /**< X component in millimeters. */
    q16_16 y; /**< Y component in millimeters. */
    q16_16 z; /**< Z component in millimeters. */
} vec3_q16;

/**
 * @brief Compute dot product of two vectors.
 * @param a First input vector.
 * @param b Second input vector.
 * @return Q16.16 dot product value.
 */
q16_16 vec3_dot(const vec3_q16 *a, const vec3_q16 *b);

/**
 * @brief Compute squared magnitude of a vector.
 * @param a Input vector.
 * @return Squared magnitude.
 */
q16_16 vec3_norm_sq(const vec3_q16 *a);

/**
 * @brief Add two vectors component-wise.
 * @param a First input vector.
 * @param b Second input vector.
 * @param out Resulting vector (may alias @p a or @p b).
 */
void vec3_add(const vec3_q16 *a, const vec3_q16 *b, vec3_q16 *out);

/**
 * @brief Subtract vector @p b from @p a component-wise.
 * @param a Minuend vector.
 * @param b Subtrahend vector.
 * @param out Resulting vector (may alias @p a or @p b).
 */
void vec3_sub(const vec3_q16 *a, const vec3_q16 *b, vec3_q16 *out);

/**
 * @brief Scale vector by scalar.
 * @param a Input vector.
 * @param scale Scalar multiplier in Q16.16.
 * @param out Resulting scaled vector.
 */
void vec3_scale(const vec3_q16 *a, q16_16 scale, vec3_q16 *out);

#endif /* UTILS_VEC3_H */
