#include "planner/splines/splines.h"

#include "utils/q16.h"
#include "utils/vec3.h"

#include <string.h>

static vec3_q16 lerp(vec3_q16 a, vec3_q16 b, q16_16 t)
{
    vec3_q16 diff = vec3_sub(b, a);
    return vec3_add(a, vec3_scale(diff, t));
}

bool spl_make_from_waypoints(const vec3_q16 *pts,
                             size_t n,
                             const q16_16 *v_in_opt,
                             const q16_16 *v_out_opt,
                             spl_plan_t *out)
{
    (void)v_in_opt;
    (void)v_out_opt;
    if ((pts == NULL) || (out == NULL) || (n == 0U) || (n > 8U))
    {
        return false;
    }
    out->type = SPL_BEZIER3;
    out->max_vel = q16_from_int(200);
    out->max_acc = q16_from_int(1000);
    out->max_jerk = q16_from_int(5000);
    out->control_count = n;
    for (size_t i = 0U; i < n; ++i)
    {
        out->control[i] = pts[i];
    }
    return true;
}

bool spl_make_brachistochrone(const vec3_q16 *start,
                              const vec3_q16 *end,
                              spl_plan_t *out)
{
    if ((start == NULL) || (end == NULL) || (out == NULL))
    {
        return false;
    }
    out->type = SPL_BRACHISTOCHRONE;
    out->max_vel = q16_from_int(300);
    out->max_acc = q16_from_int(1500);
    out->max_jerk = q16_from_int(6000);
    out->control_count = 2U;
    out->control[0] = *start;
    out->control[1] = *end;
    return true;
}

bool spl_sample_arc(const spl_plan_t *plan,
                    q16_16 s_norm,
                    vec3_q16 *xyz,
                    vec3_q16 *dxyz)
{
    if ((plan == NULL) || (xyz == NULL))
    {
        return false;
    }
    if (plan->control_count < 2U)
    {
        return false;
    }
    if (s_norm < 0)
    {
        s_norm = 0;
    }
    if (s_norm > q16_from_int(1))
    {
        s_norm = q16_from_int(1);
    }
    vec3_q16 from = plan->control[0];
    vec3_q16 to = plan->control[plan->control_count - 1U];
    *xyz = lerp(from, to, s_norm);
    if (dxyz != NULL)
    {
        vec3_q16 diff = vec3_sub(to, from);
        *dxyz = diff;
    }
    return true;
}

bool spl_time_scale(const spl_plan_t *plan,
                    q16_16 period_ms,
                    time_scaled_traj_t *out)
{
    if ((plan == NULL) || (out == NULL) || (period_ms <= 0))
    {
        return false;
    }
    q16_16 duration = q16_from_int(1000); /* 1 second default duration */
    size_t samples = (size_t)(q16_to_float(duration) / (q16_to_float(period_ms) / 1000.0f));
    if (samples < 2U)
    {
        samples = 2U;
    }
    if (samples >= 256U)
    {
        samples = 255U;
    }
    out->count = samples;
    out->period = period_ms;
    for (size_t i = 0U; i < samples; ++i)
    {
        q16_16 t = q16_div(q16_from_int((int32_t)i), q16_from_int((int32_t)(samples - 1U)));
        spl_sample_arc(plan, t, &out->position[i], &out->velocity[i]);
    }
    return true;
}
