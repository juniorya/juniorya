#ifndef PLANNER_S_CURVE_H
#define PLANNER_S_CURVE_H

#include <stdbool.h>

#include "utils/q16.h"

/** \brief Motion limits for S-curve profiles. */
typedef struct
{
    q16_16 max_vel;  /**< Maximum velocity. */
    q16_16 max_acc;  /**< Maximum acceleration. */
    q16_16 max_jerk; /**< Maximum jerk. */
} s_curve_limits;

/** \brief Summary of an S-curve profile. */
typedef struct
{
    q16_16 duration;    /**< Total duration in milliseconds. */
    q16_16 cruise_time; /**< Time spent at constant velocity. */
    q16_16 accel_time;  /**< Acceleration time. */
} s_curve_profile;

bool s_curve_plan(q16_16 distance, const s_curve_limits *limits, s_curve_profile *profile);

q16_16 s_curve_position(const s_curve_profile *profile, q16_16 t);

q16_16 s_curve_velocity(const s_curve_profile *profile, q16_16 t);

#endif
