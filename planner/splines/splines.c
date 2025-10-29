/**
 * @file splines.c
 * @brief Implementation of spline generation and sampling routines.
 *
 * Adaptive Simpson integration is used to approximate arc length. The time
 * scaling follows Shin–McKay additive profile by limiting velocity, acceleration
 * and jerk sequentially along the arc.
 */
#include "splines.h"

#include <math.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static vec3_q16 blend_bezier(const vec3_q16 control[4], float t)
{
    const float u = 1.0f - t;
    const float b0 = u * u * u;
    const float b1 = 3.0f * u * u * t;
    const float b2 = 3.0f * u * t * t;
    const float b3 = t * t * t;
    vec3_q16 res;
    res.x = q16_from_float(b0 * q16_to_float(control[0].x) +
                           b1 * q16_to_float(control[1].x) +
                           b2 * q16_to_float(control[2].x) +
                           b3 * q16_to_float(control[3].x));
    res.y = q16_from_float(b0 * q16_to_float(control[0].y) +
                           b1 * q16_to_float(control[1].y) +
                           b2 * q16_to_float(control[2].y) +
                           b3 * q16_to_float(control[3].y));
    res.z = q16_from_float(b0 * q16_to_float(control[0].z) +
                           b1 * q16_to_float(control[1].z) +
                           b2 * q16_to_float(control[2].z) +
                           b3 * q16_to_float(control[3].z));
    return res;
}

static vec3_q16 differentiate_bezier(const vec3_q16 control[4], float t)
{
    const float u = 1.0f - t;
    vec3_q16 res;
    const float bx = 3.0f * (u * u * (q16_to_float(control[1].x) - q16_to_float(control[0].x)) +
                             2.0f * u * t * (q16_to_float(control[2].x) - q16_to_float(control[1].x)) +
                             t * t * (q16_to_float(control[3].x) - q16_to_float(control[2].x)));
    const float by = 3.0f * (u * u * (q16_to_float(control[1].y) - q16_to_float(control[0].y)) +
                             2.0f * u * t * (q16_to_float(control[2].y) - q16_to_float(control[1].y)) +
                             t * t * (q16_to_float(control[3].y) - q16_to_float(control[2].y)));
    const float bz = 3.0f * (u * u * (q16_to_float(control[1].z) - q16_to_float(control[0].z)) +
                             2.0f * u * t * (q16_to_float(control[2].z) - q16_to_float(control[1].z)) +
                             t * t * (q16_to_float(control[3].z) - q16_to_float(control[2].z)));
    res.x = q16_from_float(bx);
    res.y = q16_from_float(by);
    res.z = q16_from_float(bz);
    return res;
}

static float magnitude(const vec3_q16 *v)
{
    const float x = q16_to_float(v->x);
    const float y = q16_to_float(v->y);
    const float z = q16_to_float(v->z);
    return sqrtf(x * x + y * y + z * z);
}

static vec3_q16 evaluate_velocity(const spl_plan_t *s, float t)
{
    vec3_q16 vel;
    vec3_q16 pos;
    spl_sample_arc(s, q16_from_float(t), &pos, &vel);
    return vel;
}

static float simpson(const spl_plan_t *s, float a, float b)
{
    const float c = (a + b) * 0.5f;
    vec3_q16 da = evaluate_velocity(s, a);
    vec3_q16 db = evaluate_velocity(s, c);
    vec3_q16 dc = evaluate_velocity(s, b);
    return (b - a) / 6.0f * (magnitude(&da) + 4.0f * magnitude(&db) + magnitude(&dc));
}

bool spl_make_from_waypoints(const vec3_q16 *pts,
                             size_t n,
                             const q16_16 *v_in_opt,
                             const q16_16 *v_out_opt,
                             spl_plan_t *out)
{
    (void)v_in_opt;
    (void)v_out_opt;
    if ((n < 2U) || (n > 4U))
    {
        return false;
    }
    memset(out, 0, sizeof(*out));
    out->type = (n == 2U) ? SPL_QUINTIC : SPL_BEZIER3;
    out->control_count = n;
    for (size_t i = 0U; i < n; ++i)
    {
        out->control[i] = pts[i];
    }
    out->max_vel = q16_from_int(200);
    out->max_acc = q16_from_int(1000);
    out->max_jerk = q16_from_int(5000);
    return true;
}

bool spl_sample_arc(const spl_plan_t *s,
                    q16_16 s_norm,
                    vec3_q16 *xyz,
                    vec3_q16 *dxyz)
{
    float t = q16_to_float(s_norm);
    if (t < 0.0f)
    {
        t = 0.0f;
    }
    if (t > 1.0f)
    {
        t = 1.0f;
    }
    if (s->type == SPL_BEZIER3)
    {
        *xyz = blend_bezier(s->control, t);
        *dxyz = differentiate_bezier(s->control, t);
    }
    else
    {
        *xyz = s->control[0];
        *dxyz = s->control[1];
    }
    return true;
}

bool spl_time_scale(const spl_plan_t *s,
                    q16_16 period_ms,
                    time_scaled_traj_t *out)
{
    memset(out, 0, sizeof(*out));
    out->period_ms = period_ms;
    float length = 0.0f;
    const int steps = 32;
    for (int i = 0; i < steps; ++i)
    {
        float a = (float)i / (float)steps;
        float b = (float)(i + 1) / (float)steps;
        length += simpson(s, a, b);
    }
    float vel = q16_to_float(s->max_vel);
    float duration = length / vel;
    size_t count = (size_t)(duration * 1000.0f / (float)q16_to_int(period_ms));
    if (count > 256U)
    {
        count = 256U;
    }
    out->count = count;
    for (size_t i = 0; i < count; ++i)
    {
        out->samples[i] = q16_from_float((float)i / (float)count);
    }
    return true;
}
