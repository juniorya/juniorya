#include "utils/vec3.h"

vec3_q16 vec3_add(vec3_q16 a, vec3_q16 b)
{
    vec3_q16 r;
    r.x = a.x + b.x;
    r.y = a.y + b.y;
    r.z = a.z + b.z;
    return r;
}

vec3_q16 vec3_sub(vec3_q16 a, vec3_q16 b)
{
    vec3_q16 r;
    r.x = a.x - b.x;
    r.y = a.y - b.y;
    r.z = a.z - b.z;
    return r;
}

vec3_q16 vec3_scale(vec3_q16 v, q16_16 s)
{
    vec3_q16 r;
    r.x = q16_mul(v.x, s);
    r.y = q16_mul(v.y, s);
    r.z = q16_mul(v.z, s);
    return r;
}

q16_16 vec3_dot(vec3_q16 a, vec3_q16 b)
{
    q16_16 x = q16_mul(a.x, b.x);
    q16_16 y = q16_mul(a.y, b.y);
    q16_16 z = q16_mul(a.z, b.z);
    return x + y + z;
}

q16_16 vec3_length_sq(vec3_q16 v)
{
    return vec3_dot(v, v);
}

vec3_q16 vec3_normalise(vec3_q16 v)
{
    q16_16 len_sq = vec3_length_sq(v);
    if (len_sq <= 0)
    {
        vec3_q16 zero = {0, 0, 0};
        return zero;
    }
    q16_16 inv_len = q16_div(q16_from_int(1), q16_sqrt(len_sq));
    return vec3_scale(v, inv_len);
}
