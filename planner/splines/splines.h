/**
 * @file splines.h
 * @brief Geometric spline planner supporting Bézier, B-spline, quintic and NURBS-lite curves.
 */

#ifndef SPLINES_H
#define SPLINES_H

#include <stdbool.h>
#include <stddef.h>

#include "utils/vec3.h"

#ifndef ENABLE_BRACHISTO
#define ENABLE_BRACHISTO 0
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Supported spline types. */
typedef enum
{
    SPL_BEZIER3 = 0,      /**< Cubic Bézier curve. */
    SPL_BSPLINE3,         /**< Cubic uniform B-spline. */
    SPL_QUINTIC,          /**< C³ quintic polynomial interpolant. */
    SPL_NURBS3,           /**< Rational cubic curve with uniform weights. */
    SPL_BRACHISTOCHRONE   /**< Cycloidal brachistochrone trajectory. */
} spl_type_t;

/** @brief Precomputed spline representation. */
typedef struct
{
    spl_type_t type;        /**< Curve type. */
    q16_16 max_vel;         /**< Velocity limit along curve. */
    q16_16 max_acc;         /**< Acceleration limit. */
    q16_16 max_jerk;        /**< Jerk limit. */
    vec3_q16 control[8];    /**< Control points or coefficients. */
    size_t control_count;   /**< Number of control points. */
    q16_16 knots[12];       /**< Knot vector for B-spline/NURBS. */
    q16_16 length;          /**< Approximated arc length. */
    q16_16 brachisto_theta; /**< Terminal cycloidal parameter (radians, Q16.16). */
    q16_16 brachisto_radius;/**< Generating circle radius for brachistochrone. */
    vec3_q16 brachisto_dir_h; /**< Unit horizontal direction for brachistochrone. */
    vec3_q16 brachisto_dir_v; /**< Unit vertical direction (gravity) for brachistochrone. */
} spl_plan_t;

/** @brief Time-scaled trajectory sample sequence. */
typedef struct
{
    q16_16 period_ms;       /**< Sampling period. */
    size_t count;           /**< Number of samples. */
    vec3_q16 position[256]; /**< Sampled positions. */
    vec3_q16 velocity[256]; /**< Sampled derivative vectors. */
} time_scaled_traj_t;

/**
 * @brief Build spline plan from waypoints and optional boundary velocities.
 * @param pts Array of waypoints.
 * @param n Number of waypoints.
 * @param v_in_opt Optional entry velocity array or NULL.
 * @param v_out_opt Optional exit velocity array or NULL.
 * @param out Output plan.
 * @return True on success.
 */
bool spl_make_from_waypoints(const vec3_q16 *pts,
                             size_t n,
                             const q16_16 *v_in_opt,
                             const q16_16 *v_out_opt,
                             spl_plan_t *out);

/**
 * @brief Sample spline by normalised arc length parameter.
 * @param s Plan to sample.
 * @param s_norm Normalised progress [0,1].
 * @param xyz Output position.
 * @param dxyz Output derivative.
 * @return True on success.
 */
bool spl_sample_arc(const spl_plan_t *s, q16_16 s_norm, vec3_q16 *xyz, vec3_q16 *dxyz);

/**
 * @brief Perform Shin–McKay additive time scaling.
 * @param s Spline plan.
 * @param period_ms Sampling period.
 * @param out Output time-scaled trajectory.
 * @return True on success.
 */
bool spl_time_scale(const spl_plan_t *s, q16_16 period_ms, time_scaled_traj_t *out);

/**
 * @brief Build brachistochrone trajectory between two poses.
 *
 * The trajectory follows the classical cycloidal solution of Johann Bernoulli's
 * brachistochrone problem projected into the plane defined by the horizontal
 * displacement of the endpoints and the gravity vector. The feature is
 * available only when the build is configured with ENABLE_BRACHISTO=1.
 *
 * @param start Entry pose.
 * @param end Exit pose (must be lower than @p start along the gravity axis).
 * @param out Output plan populated on success.
 * @return True when a valid brachistochrone can be formed.
 */
#if ENABLE_BRACHISTO
bool spl_make_brachistochrone(const vec3_q16 *start, const vec3_q16 *end, spl_plan_t *out);
#else
static inline bool spl_make_brachistochrone(const vec3_q16 *start,
                                            const vec3_q16 *end,
                                            spl_plan_t *out)
{
    (void)start;
    (void)end;
    (void)out;
    return false;
}
#endif

#ifdef __cplusplus
}
#endif

#endif
