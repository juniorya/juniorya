/**
 * @file vec3.h
 * @brief Three-dimensional vector utilities for Q16.16 arithmetic.
 */

#ifndef VEC3_H
#define VEC3_H

#include "q16.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Representation of a 3D vector in fixed-point format. */
typedef struct
{
    q16_16 x; /**< X component in Q16.16. */
    q16_16 y; /**< Y component in Q16.16. */
    q16_16 z; /**< Z component in Q16.16. */
} vec3_q16;

/**
 * @brief Initialise vector from floating-point coordinates.
 * @param x X coordinate.
 * @param y Y coordinate.
 * @param z Z coordinate.
 * @return Vector with Q16.16 components.
 */
vec3_q16 vec3_from_float(float x, float y, float z);

/**
 * @brief Add two vectors component-wise.
 * @param a First operand.
 * @param b Second operand.
 * @return Component-wise sum.
 */
vec3_q16 vec3_add(vec3_q16 a, vec3_q16 b);

/**
 * @brief Subtract two vectors component-wise.
 * @param a Minuend.
 * @param b Subtrahend.
 * @return Component-wise difference.
 */
vec3_q16 vec3_sub(vec3_q16 a, vec3_q16 b);

/**
 * @brief Scale vector by scalar.
 * @param v Input vector.
 * @param scale Scaling factor in Q16.16.
 * @return Scaled vector.
 */
vec3_q16 vec3_scale(vec3_q16 v, q16_16 scale);

/**
 * @brief Compute dot product.
 * @param a First vector.
 * @param b Second vector.
 * @return Dot product in Q16.16.
 */
q16_16 vec3_dot(vec3_q16 a, vec3_q16 b);

/**
 * @brief Compute squared norm.
 * @param v Input vector.
 * @return Squared magnitude in Q16.16.
 */
q16_16 vec3_norm_sq(vec3_q16 v);

/**
 * @brief Compute Euclidean length.
 * @param v Input vector.
 * @return Vector length in Q16.16.
 */
q16_16 vec3_norm(vec3_q16 v);

/**
 * @brief Cross product.
 * @param a First vector.
 * @param b Second vector.
 * @return Cross product vector.
 */
vec3_q16 vec3_cross(vec3_q16 a, vec3_q16 b);

#ifdef __cplusplus
}
#endif

#endif
