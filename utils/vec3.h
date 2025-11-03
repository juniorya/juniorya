#ifndef VEC3_H
#define VEC3_H

#include "utils/q16.h"

/** \brief Q16.16 3D vector. */
typedef struct
{
    q16_16 x; /**< X component. */
    q16_16 y; /**< Y component. */
    q16_16 z; /**< Z component. */
} vec3_q16;

/** \brief Add two vectors. */
vec3_q16 vec3_add(vec3_q16 a, vec3_q16 b);

/** \brief Subtract two vectors. */
vec3_q16 vec3_sub(vec3_q16 a, vec3_q16 b);

/** \brief Scale vector by scalar. */
vec3_q16 vec3_scale(vec3_q16 v, q16_16 s);

/** \brief Dot product of two vectors. */
q16_16 vec3_dot(vec3_q16 a, vec3_q16 b);

/** \brief Length squared of a vector. */
q16_16 vec3_length_sq(vec3_q16 v);

/** \brief Normalise vector (returns zero vector on failure). */
vec3_q16 vec3_normalise(vec3_q16 v);

#endif
