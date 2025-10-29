/**
 * @file splines.c
 * @brief Implementation of geometric spline planner.
 */

#include "splines.h"

#include "q16.h"

#include <math.h>
#include <string.h>

static float to_float(q16_16 value)
{
    return q16_to_float(value);
}

static q16_16 from_float(float value)
{
    return q16_from_float(value);
}

static vec3_q16 vec_from_float(float x, float y, float z)
{
    vec3_q16 v;
    v.x = from_float(x);
    v.y = from_float(y);
    v.z = from_float(z);
    return v;
}

static vec3_q16 eval_bezier3(const spl_plan_t *s, float t, vec3_q16 *dxyz)
{
    float u = 1.0f - t;
    float b0 = u * u * u;
    float b1 = 3.0f * u * u * t;
    float b2 = 3.0f * u * t * t;
    float b3 = t * t * t;
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    for (int i = 0; i < 4; ++i)
    {
        float coeff = (i == 0) ? b0 : (i == 1) ? b1 : (i == 2) ? b2 : b3;
        x += coeff * to_float(s->control[i].x);
        y += coeff * to_float(s->control[i].y);
        z += coeff * to_float(s->control[i].z);
    }
    if (dxyz != NULL)
    {
        float dx = -3.0f * u * u * to_float(s->control[0].x) +
                   3.0f * (u * u - 2.0f * u * t) * to_float(s->control[1].x) +
                   3.0f * (2.0f * u * t - t * t) * to_float(s->control[2].x) +
                   3.0f * t * t * to_float(s->control[3].x);
        float dy = -3.0f * u * u * to_float(s->control[0].y) +
                   3.0f * (u * u - 2.0f * u * t) * to_float(s->control[1].y) +
                   3.0f * (2.0f * u * t - t * t) * to_float(s->control[2].y) +
                   3.0f * t * t * to_float(s->control[3].y);
        float dz = -3.0f * u * u * to_float(s->control[0].z) +
                   3.0f * (u * u - 2.0f * u * t) * to_float(s->control[1].z) +
                   3.0f * (2.0f * u * t - t * t) * to_float(s->control[2].z) +
                   3.0f * t * t * to_float(s->control[3].z);
        *dxyz = vec_from_float(dx, dy, dz);
    }
    return vec_from_float(x, y, z);
}

static float basis_bspline(int i, int k, float t, const float *knots)
{
    if (k == 1)
    {
        return (knots[i] <= t && t < knots[i + 1]) ? 1.0f : 0.0f;
    }
    float denom1 = knots[i + k - 1] - knots[i];
    float denom2 = knots[i + k] - knots[i + 1];
    float term1 = 0.0f;
    float term2 = 0.0f;
    if (denom1 > 0.0f)
    {
        term1 = (t - knots[i]) / denom1 * basis_bspline(i, k - 1, t, knots);
    }
    if (denom2 > 0.0f)
    {
        term2 = (knots[i + k] - t) / denom2 * basis_bspline(i + 1, k - 1, t, knots);
    }
    return term1 + term2;
}

static vec3_q16 eval_bspline3(const spl_plan_t *s, float t, vec3_q16 *dxyz)
{
    float knots[12];
    for (size_t i = 0; i < s->control_count + 4; ++i)
    {
        knots[i] = (float)i / (float)(s->control_count + 4 - 1);
    }
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    for (size_t i = 0; i < s->control_count; ++i)
    {
        float b = basis_bspline((int)i, 4, t, knots);
        x += b * to_float(s->control[i].x);
        y += b * to_float(s->control[i].y);
        z += b * to_float(s->control[i].z);
    }
    if (dxyz != NULL)
    {
        vec3_q16 forward = eval_bspline3(s, fminf(t + 0.001f, 1.0f), NULL);
        vec3_q16 backward = eval_bspline3(s, fmaxf(t - 0.001f, 0.0f), NULL);
        dxyz->x = q16_sub(forward.x, backward.x);
        dxyz->y = q16_sub(forward.y, backward.y);
        dxyz->z = q16_sub(forward.z, backward.z);
    }
    return vec_from_float(x, y, z);
}

static vec3_q16 eval_quintic(const spl_plan_t *s, float t, vec3_q16 *dxyz)
{
    const vec3_q16 p0 = s->control[0];
    const vec3_q16 p1 = s->control[1];
    const vec3_q16 v0 = s->control[2];
    const vec3_q16 v1 = s->control[3];
    const vec3_q16 a0 = s->control[4];
    const vec3_q16 a1 = s->control[5];
    float tt = t * t;
    float t3 = tt * t;
    float t4 = t3 * t;
    float t5 = t4 * t;
    float h0 = 1.0f - 10.0f * t3 + 15.0f * t4 - 6.0f * t5;
    float h1 = t - 6.0f * tt + 8.0f * t3 - 3.0f * t4;
    float h2 = 0.5f * tt - 1.5f * t3 + 1.5f * t4 - 0.5f * t5;
    float h3 = 10.0f * t3 - 15.0f * t4 + 6.0f * t5;
    float h4 = -4.0f * t3 + 7.0f * t4 - 3.0f * t5;
    float h5 = 0.5f * t3 - t4 + 0.5f * t5;
    float x = h0 * to_float(p0.x) + h1 * to_float(v0.x) + h2 * to_float(a0.x) +
              h3 * to_float(p1.x) + h4 * to_float(v1.x) + h5 * to_float(a1.x);
    float y = h0 * to_float(p0.y) + h1 * to_float(v0.y) + h2 * to_float(a0.y) +
              h3 * to_float(p1.y) + h4 * to_float(v1.y) + h5 * to_float(a1.y);
    float z = h0 * to_float(p0.z) + h1 * to_float(v0.z) + h2 * to_float(a0.z) +
              h3 * to_float(p1.z) + h4 * to_float(v1.z) + h5 * to_float(a1.z);
    if (dxyz != NULL)
    {
        float dh0 = -30.0f * tt + 60.0f * t3 - 30.0f * t4;
        float dh1 = 1.0f - 12.0f * t + 24.0f * tt - 12.0f * t3;
        float dh2 = tt - 4.5f * t3 + 6.0f * t4 - 2.5f * t5;
        float dh3 = 30.0f * tt - 60.0f * t3 + 30.0f * t4;
        float dh4 = -12.0f * tt + 28.0f * t3 - 15.0f * t4;
        float dh5 = 1.5f * tt - 4.0f * t3 + 2.5f * t4;
        float dx = dh0 * to_float(p0.x) + dh1 * to_float(v0.x) + dh2 * to_float(a0.x) +
                   dh3 * to_float(p1.x) + dh4 * to_float(v1.x) + dh5 * to_float(a1.x);
        float dy = dh0 * to_float(p0.y) + dh1 * to_float(v0.y) + dh2 * to_float(a0.y) +
                   dh3 * to_float(p1.y) + dh4 * to_float(v1.y) + dh5 * to_float(a1.y);
        float dz = dh0 * to_float(p0.z) + dh1 * to_float(v0.z) + dh2 * to_float(a0.z) +
                   dh3 * to_float(p1.z) + dh4 * to_float(v1.z) + dh5 * to_float(a1.z);
        *dxyz = vec_from_float(dx, dy, dz);
    }
    return vec_from_float(x, y, z);
}

static vec3_q16 eval_nurbs(const spl_plan_t *s, float t, vec3_q16 *dxyz)
{
    float numerator_x = 0.0f;
    float numerator_y = 0.0f;
    float numerator_z = 0.0f;
    float denominator = 0.0f;
    float knots[12];
    for (size_t i = 0; i < s->control_count + 4; ++i)
    {
        knots[i] = (float)i / (float)(s->control_count + 4 - 1);
    }
    for (size_t i = 0; i < s->control_count; ++i)
    {
        float b = basis_bspline((int)i, 4, t, knots);
        float weight = 1.0f;
        numerator_x += b * weight * to_float(s->control[i].x);
        numerator_y += b * weight * to_float(s->control[i].y);
        numerator_z += b * weight * to_float(s->control[i].z);
        denominator += b * weight;
    }
    if (denominator <= 0.0f)
    {
        denominator = 1.0f;
    }
    float x = numerator_x / denominator;
    float y = numerator_y / denominator;
    float z = numerator_z / denominator;
    if (dxyz != NULL)
    {
        vec3_q16 fwd = eval_nurbs(s, fminf(t + 0.001f, 1.0f), NULL);
        vec3_q16 back = eval_nurbs(s, fmaxf(t - 0.001f, 0.0f), NULL);
        dxyz->x = q16_sub(fwd.x, back.x);
        dxyz->y = q16_sub(fwd.y, back.y);
        dxyz->z = q16_sub(fwd.z, back.z);
    }
    return vec_from_float(x, y, z);
}

static float derivative_magnitude(const spl_plan_t *s, float t)
{
    vec3_q16 deriv;
    switch (s->type)
    {
        case SPL_BEZIER3:
            (void)eval_bezier3(s, t, &deriv);
            break;
        case SPL_BSPLINE3:
            (void)eval_bspline3(s, t, &deriv);
            break;
        case SPL_QUINTIC:
            (void)eval_quintic(s, t, &deriv);
            break;
        case SPL_NURBS3:
        default:
            (void)eval_nurbs(s, t, &deriv);
            break;
    }
    float dx = to_float(deriv.x);
    float dy = to_float(deriv.y);
    float dz = to_float(deriv.z);
    return sqrtf(dx * dx + dy * dy + dz * dz);
}

static float arc_length(const spl_plan_t *s)
{
    const int steps = 128;
    float sum = 0.0f;
    for (int i = 0; i < steps; ++i)
    {
        float a = (float)i / (float)steps;
        float b = (float)(i + 1) / (float)steps;
        float c = (a + b) * 0.5f;
        float fa = derivative_magnitude(s, a);
        float fb = derivative_magnitude(s, b);
        float fc = derivative_magnitude(s, c);
        sum += (b - a) / 6.0f * (fa + 4.0f * fc + fb);
    }
    return sum;
}

bool spl_make_from_waypoints(const vec3_q16 *pts,
                             size_t n,
                             const q16_16 *v_in_opt,
                             const q16_16 *v_out_opt,
                             spl_plan_t *out)
{
    memset(out, 0, sizeof(*out));
    if (n == 4U)
    {
        out->type = SPL_BEZIER3;
    }
    else if (n >= 4U)
    {
        out->type = SPL_BSPLINE3;
    }
    else if (n == 2U)
    {
        out->type = SPL_QUINTIC;
    }
    else
    {
        return false;
    }
    out->max_vel = q16_from_int(200);
    out->max_acc = q16_from_int(1000);
    out->max_jerk = q16_from_int(5000);
    out->control_count = n;
    for (size_t i = 0; i < n; ++i)
    {
        out->control[i] = pts[i];
    }
    if (out->type == SPL_QUINTIC)
    {
        vec3_q16 start = pts[0];
        vec3_q16 end = pts[1];
        vec3_q16 zero = vec3_from_float(0.0f, 0.0f, 0.0f);
        vec3_q16 start_vel = zero;
        vec3_q16 end_vel = zero;
        if (v_in_opt != NULL)
        {
            q16_16 v = v_in_opt[0];
            start_vel.x = v;
            start_vel.y = v;
            start_vel.z = v;
        }
        if (v_out_opt != NULL)
        {
            q16_16 v = v_out_opt[0];
            end_vel.x = v;
            end_vel.y = v;
            end_vel.z = v;
        }
        out->control_count = 6U;
        out->control[0] = start;
        out->control[1] = end;
        out->control[2] = start_vel;
        out->control[3] = end_vel;
        out->control[4] = zero;
        out->control[5] = zero;
    }
    out->length = from_float(arc_length(out));
    return true;
}

bool spl_sample_arc(const spl_plan_t *s, q16_16 s_norm, vec3_q16 *xyz, vec3_q16 *dxyz)
{
    float t = to_float(s_norm);
    if (t < 0.0f)
    {
        t = 0.0f;
    }
    if (t > 1.0f)
    {
        t = 1.0f;
    }
    switch (s->type)
    {
        case SPL_BEZIER3:
            *xyz = eval_bezier3(s, t, dxyz);
            break;
        case SPL_BSPLINE3:
            *xyz = eval_bspline3(s, t, dxyz);
            break;
        case SPL_QUINTIC:
            *xyz = eval_quintic(s, t, dxyz);
            break;
        case SPL_NURBS3:
        default:
            *xyz = eval_nurbs(s, t, dxyz);
            break;
    }
    return true;
}

bool spl_time_scale(const spl_plan_t *s, q16_16 period_ms, time_scaled_traj_t *out)
{
    out->period_ms = period_ms;
    float length = to_float(s->length);
    if (length <= 0.0f)
    {
        length = arc_length(s);
    }
    float max_vel = to_float(s->max_vel);
    float duration = length / (max_vel > 1e-6f ? max_vel : 1.0f);
    size_t count = (size_t)(duration * 1000.0f / to_float(period_ms));
    if (count < 2U)
    {
        count = 2U;
    }
    if (count > 256U)
    {
        count = 256U;
    }
    out->count = count;
    for (size_t i = 0; i < count; ++i)
    {
        float s_norm = (float)i / (float)(count - 1);
        vec3_q16 pos;
        vec3_q16 deriv;
        spl_sample_arc(s, from_float(s_norm), &pos, &deriv);
        out->position[i] = pos;
        out->velocity[i] = deriv;
    }
    return true;
}
