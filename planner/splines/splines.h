#ifndef PLANNER_SPLINES_H
#define PLANNER_SPLINES_H

#include <stdbool.h>
#include <stddef.h>

#include "utils/vec3.h"

/** \brief Supported spline types. */
typedef enum
{
    SPL_BEZIER3,   /**< Cubic Bézier spline. */
    SPL_BSPLINE3,  /**< Cubic uniform B-spline. */
    SPL_QUINTIC,   /**< Quintic polynomial. */
    SPL_NURBS3,    /**< Rational cubic spline. */
    SPL_BRACHISTOCHRONE /**< Optional brachistochrone arc. */
} spl_type_t;

/** \brief Time scaled trajectory sample buffer. */
typedef struct
{
    vec3_q16 position[256]; /**< Position samples. */
    vec3_q16 velocity[256]; /**< Velocity samples. */
    size_t count;           /**< Number of valid samples. */
    q16_16 period;          /**< Sampling period. */
} time_scaled_traj_t;

/** \brief Prepared spline plan. */
typedef struct
{
    spl_type_t type;        /**< Spline family. */
    q16_16 max_vel;         /**< Velocity limit. */
    q16_16 max_acc;         /**< Acceleration limit. */
    q16_16 max_jerk;        /**< Jerk limit. */
    vec3_q16 control[8];    /**< Control points. */
    size_t control_count;   /**< Number of control points. */
} spl_plan_t;

bool spl_make_from_waypoints(const vec3_q16 *pts,
                             size_t n,
                             const q16_16 *v_in_opt,
                             const q16_16 *v_out_opt,
                             spl_plan_t *out);

bool spl_make_brachistochrone(const vec3_q16 *start,
                              const vec3_q16 *end,
                              spl_plan_t *out);

bool spl_sample_arc(const spl_plan_t *plan,
                    q16_16 s_norm,
                    vec3_q16 *xyz,
                    vec3_q16 *dxyz);

bool spl_time_scale(const spl_plan_t *plan,
                    q16_16 period_ms,
                    time_scaled_traj_t *out);

#endif
