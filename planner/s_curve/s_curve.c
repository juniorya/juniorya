/**
 * @file s_curve.c
 * @brief Jerk-limited motion profile implementation.
 */

#include "s_curve.h"

#include "q16.h"

void s_curve_generate(s_curve_profile *profile,
                      q16_16 max_vel,
                      q16_16 max_acc,
                      q16_16 max_jerk,
                      const spl_plan_t *plan)
{
    profile->max_vel = max_vel;
    profile->max_acc = max_acc;
    profile->max_jerk = max_jerk;
    profile->position = 0;
    profile->velocity = 0;
    profile->acceleration = 0;
    profile->jerk = max_jerk;
    profile->dt_ms = q16_from_int(1);
    profile->complete = (plan == NULL);
}

void s_curve_step(s_curve_profile *profile)
{
    if (profile->complete)
    {
        return;
    }
    q16_16 dt = profile->dt_ms;
    profile->acceleration = q16_add(profile->acceleration, q16_mul(profile->jerk, dt));
    if (q16_abs(profile->acceleration) > profile->max_acc)
    {
        profile->acceleration = profile->max_acc;
        profile->jerk = 0;
    }
    profile->velocity = q16_add(profile->velocity, q16_mul(profile->acceleration, dt));
    if (q16_abs(profile->velocity) > profile->max_vel)
    {
        profile->velocity = profile->max_vel;
    }
    profile->position = q16_add(profile->position, q16_mul(profile->velocity, dt));
    if (profile->velocity == profile->max_vel)
    {
        profile->complete = true;
    }
}
