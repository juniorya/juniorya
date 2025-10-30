/**
 * @file vec3.c
 * @brief Implementation of fixed-point vector utilities.
 */

#include "vec3.h"

vec3_q16 vec3_from_float(float x, float y, float z)
{
    vec3_q16 out;
    out.x = q16_from_float(x);
    out.y = q16_from_float(y);
    out.z = q16_from_float(z);
    return out;
}

vec3_q16 vec3_add(vec3_q16 a, vec3_q16 b)
{
    vec3_q16 out;
    out.x = q16_add(a.x, b.x);
    out.y = q16_add(a.y, b.y);
    out.z = q16_add(a.z, b.z);
    return out;
}

vec3_q16 vec3_sub(vec3_q16 a, vec3_q16 b)
{
    vec3_q16 out;
    out.x = q16_sub(a.x, b.x);
    out.y = q16_sub(a.y, b.y);
    out.z = q16_sub(a.z, b.z);
    return out;
}

vec3_q16 vec3_scale(vec3_q16 v, q16_16 scale)
{
    vec3_q16 out;
    out.x = q16_mul(v.x, scale);
    out.y = q16_mul(v.y, scale);
    out.z = q16_mul(v.z, scale);
    return out;
}

q16_16 vec3_dot(vec3_q16 a, vec3_q16 b)
{
    q16_16 rx = q16_mul(a.x, b.x);
    q16_16 ry = q16_mul(a.y, b.y);
    q16_16 rz = q16_mul(a.z, b.z);
    return q16_add(q16_add(rx, ry), rz);
}

q16_16 vec3_norm_sq(vec3_q16 v)
{
    return vec3_dot(v, v);
}

q16_16 vec3_norm(vec3_q16 v)
{
    return q16_sqrt(vec3_norm_sq(v));
}

vec3_q16 vec3_cross(vec3_q16 a, vec3_q16 b)
{
    vec3_q16 out;
    out.x = q16_sub(q16_mul(a.y, b.z), q16_mul(a.z, b.y));
    out.y = q16_sub(q16_mul(a.z, b.x), q16_mul(a.x, b.z));
    out.z = q16_sub(q16_mul(a.x, b.y), q16_mul(a.y, b.x));
    return out;
}
