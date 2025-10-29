/**
 * @file vec3.c
 * @brief Implementation of Q16.16 vector helpers.
 *
 * All routines are pure functions without side effects which simplifies unit
 * testing. They rely on the saturated arithmetic helpers from q16.h to guard
 * against overflow on fixed-point platforms.
 */
#include "vec3.h"

q16_16 vec3_dot(const vec3_q16 *a, const vec3_q16 *b)
{
    q16_16 x = q16_mul(a->x, b->x);
    q16_16 y = q16_mul(a->y, b->y);
    q16_16 z = q16_mul(a->z, b->z);
    return q16_add(q16_add(x, y), z);
}

q16_16 vec3_norm_sq(const vec3_q16 *a)
{
    return vec3_dot(a, a);
}

void vec3_add(const vec3_q16 *a, const vec3_q16 *b, vec3_q16 *out)
{
    out->x = q16_add(a->x, b->x);
    out->y = q16_add(a->y, b->y);
    out->z = q16_add(a->z, b->z);
}

void vec3_sub(const vec3_q16 *a, const vec3_q16 *b, vec3_q16 *out)
{
    out->x = q16_sub(a->x, b->x);
    out->y = q16_sub(a->y, b->y);
    out->z = q16_sub(a->z, b->z);
}

void vec3_scale(const vec3_q16 *a, q16_16 scale, vec3_q16 *out)
{
    out->x = q16_mul(a->x, scale);
    out->y = q16_mul(a->y, scale);
    out->z = q16_mul(a->z, scale);
}
