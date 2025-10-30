/**
 * @file s_curve.h
 * @brief Jerk-limited S-curve profile generator.
 */

#ifndef S_CURVE_H
#define S_CURVE_H

#include <stdbool.h>

#include "planner/splines/splines.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief S-curve segment state. */
typedef struct
{
    q16_16 max_vel;     /**< Velocity limit. */
    q16_16 max_acc;     /**< Acceleration limit. */
    q16_16 max_jerk;    /**< Jerk limit. */
    q16_16 position;    /**< Current position along trajectory. */
    q16_16 velocity;    /**< Current velocity. */
    q16_16 acceleration;/**< Current acceleration. */
    q16_16 jerk;        /**< Current jerk. */
    q16_16 dt_ms;       /**< Integration timestep in milliseconds. */
    bool complete;      /**< Flag when motion complete. */
} s_curve_profile;

/**
 * @brief Generate S-curve profile for spline plan.
 * @param profile Output profile.
 * @param max_vel Velocity limit.
 * @param max_acc Acceleration limit.
 * @param max_jerk Jerk limit.
 * @param plan Spline plan.
 */
void s_curve_generate(s_curve_profile *profile,
                      q16_16 max_vel,
                      q16_16 max_acc,
                      q16_16 max_jerk,
                      const spl_plan_t *plan);

/**
 * @brief Advance S-curve profile by one timestep.
 * @param profile Profile to integrate.
 */
void s_curve_step(s_curve_profile *profile);

#ifdef __cplusplus
}
#endif

#endif
