#include "planner/s_curve/s_curve.h"

#include <math.h>
#include <stddef.h>

bool s_curve_plan(q16_16 distance, const s_curve_limits *limits, s_curve_profile *profile)
{
    if ((limits == NULL) || (profile == NULL) || (distance <= 0))
    {
        return false;
    }
    float dist = q16_to_float(distance);
    float vmax = q16_to_float(limits->max_vel);
    float acc = q16_to_float(limits->max_acc);
    float t_acc = vmax / acc;
    float d_acc = 0.5f * acc * t_acc * t_acc;
    float cruise = 0.0f;
    if (dist > (2.0f * d_acc))
    {
        cruise = (dist - (2.0f * d_acc)) / vmax;
    }
    float total = (2.0f * t_acc) + cruise;
    profile->accel_time = q16_from_float(t_acc * 1000.0f);
    profile->cruise_time = q16_from_float(cruise * 1000.0f);
    profile->duration = q16_from_float(total * 1000.0f);
    return true;
}

q16_16 s_curve_position(const s_curve_profile *profile, q16_16 t)
{
    if ((profile == NULL) || (t < 0))
    {
        return 0;
    }
    float time_ms = q16_to_float(t);
    float acc_ms = q16_to_float(profile->accel_time);
    float cruise_ms = q16_to_float(profile->cruise_time);
    float total = q16_to_float(profile->duration);
    float pos = 0.0f;
    if (time_ms <= acc_ms)
    {
        float t_s = time_ms / 1000.0f;
        float acc = (float)q16_to_float(profile->accel_time) / 1000.0f;
        pos = 0.5f * acc * t_s * t_s;
    }
    else if (time_ms <= (acc_ms + cruise_ms))
    {
        float t_s = (time_ms - acc_ms) / 1000.0f;
        float acc = acc_ms / 1000.0f;
        float vel = acc;
        pos = 0.5f * acc * (acc_ms / 1000.0f) * (acc_ms / 1000.0f) + vel * t_s;
    }
    else if (time_ms <= total)
    {
        float t_s = (time_ms - acc_ms - cruise_ms) / 1000.0f;
        float acc = acc_ms / 1000.0f;
        float vel = acc;
        float cruise_dist = vel * (cruise_ms / 1000.0f);
        float start = 0.5f * acc * (acc_ms / 1000.0f) * (acc_ms / 1000.0f) + cruise_dist;
        pos = start + vel * t_s - 0.5f * acc * t_s * t_s;
    }
    else
    {
        pos = total / 1000.0f;
    }
    return q16_from_float(pos);
}

q16_16 s_curve_velocity(const s_curve_profile *profile, q16_16 t)
{
    if ((profile == NULL) || (t < 0))
    {
        return 0;
    }
    float time_ms = q16_to_float(t);
    float acc_ms = q16_to_float(profile->accel_time);
    float cruise_ms = q16_to_float(profile->cruise_time);
    float total = q16_to_float(profile->duration);
    float vel = 0.0f;
    if (time_ms <= acc_ms)
    {
        vel = time_ms / 1000.0f;
    }
    else if (time_ms <= (acc_ms + cruise_ms))
    {
        vel = acc_ms / 1000.0f;
    }
    else if (time_ms <= total)
    {
        float t_s = (time_ms - acc_ms - cruise_ms) / 1000.0f;
        vel = (acc_ms / 1000.0f) - t_s;
    }
    return q16_from_float(vel);
}
