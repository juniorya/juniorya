/**
 * @file splines.h
 * @brief Parametric spline generator with arc-length sampling and time-scaling.
 *
 * The spline module supports cubic Bézier, cubic B-spline, quintic polynomial,
 * and rational cubic (NURBS-lite) trajectories. Each plan embeds the coefficients
 * necessary to evaluate the curve and its derivative. Arc-length sampling is
 * performed using adaptive Simpson integration which yields deterministic
 * precision with a small lookup table.
 */
#ifndef PLANNER_SPLINES_SPLINES_H
#define PLANNER_SPLINES_SPLINES_H

#include <stddef.h>
#include <stdbool.h>

#include "utils/vec3.h"

/**
 * @brief Supported spline types.
 */
typedef enum
{
    SPL_BEZIER3 = 0, /**< Cubic Bézier curve. */
    SPL_BSPLINE3,    /**< Uniform cubic B-spline. */
    SPL_QUINTIC,     /**< Quintic polynomial with C3 continuity. */
    SPL_NURBS3       /**< Rational cubic spline with unit weights by default. */
} spl_type_t;

/**
 * @brief Time-scaled trajectory output.
 */
typedef struct
{
    q16_16 period_ms; /**< Control period in milliseconds. */
    q16_16 samples[256]; /**< Normalized arc length samples. */
    size_t count; /**< Number of valid samples. */
} time_scaled_traj_t;

/**
 * @brief Spline plan containing precomputed coefficients.
 */
typedef struct
{
    spl_type_t type;          /**< Spline type. */
    q16_16 max_vel;           /**< Global velocity limit. */
    q16_16 max_acc;           /**< Global acceleration limit. */
    q16_16 max_jerk;          /**< Global jerk limit. */
    vec3_q16 control[8];      /**< Control points or polynomial coefficients. */
    q16_16 knots[8];          /**< Knot vector for B-spline/NURBS. */
    size_t control_count;     /**< Number of control points. */
} spl_plan_t;

bool spl_make_from_waypoints(const vec3_q16 *pts,
                             size_t n,
                             const q16_16 *v_in_opt,
                             const q16_16 *v_out_opt,
                             spl_plan_t *out);

bool spl_sample_arc(const spl_plan_t *s,
                    q16_16 s_norm,
                    vec3_q16 *xyz,
                    vec3_q16 *dxyz);

bool spl_time_scale(const spl_plan_t *s,
                    q16_16 period_ms,
                    time_scaled_traj_t *out);

#endif /* PLANNER_SPLINES_SPLINES_H */
