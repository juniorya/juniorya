/**
 * @file planner.h
 * @brief Motion planner providing look-ahead and jerk-limited S-curve profiles.
 *
 * The planner ingests linear and spline segments in Cartesian space and
 * generates synchronized set-points for the 1 kHz control loop. Jerk-limited
 * profiles are implemented using the standard seven-phase trajectory as described
 * by Shin and McKay (1986). Splines are integrated through precomputed arc-length
 * tables and the time-scaling algorithm also described by Shin and McKay.
 */
#ifndef PLANNER_PLANNER_H
#define PLANNER_PLANNER_H

#include <stddef.h>
#include <stdbool.h>

#include "utils/vec3.h"
#include "planner/splines/splines.h"

/**
 * @brief Planner segment types.
 */
typedef enum
{
    PLANNER_SEG_LINEAR = 0, /**< Linear interpolation between waypoints. */
    PLANNER_SEG_SPLINE      /**< Reference to a spline segment. */
} planner_segment_type_t;

/**
 * @brief Planner configuration for jerk-limited motion.
 */
typedef struct
{
    q16_16 max_velocity; /**< Maximum velocity in mm/s. */
    q16_16 max_accel;    /**< Maximum acceleration in mm/s^2. */
    q16_16 max_jerk;     /**< Maximum jerk in mm/s^3. */
} planner_limits_t;

/**
 * @brief Planner segment representation.
 */
typedef struct
{
    planner_segment_type_t type; /**< Segment type. */
    vec3_q16 start;              /**< Start point. */
    vec3_q16 end;                /**< End point for linear segments. */
    spl_plan_t spline;           /**< Embedded spline description. */
    q16_16 duration_ms;          /**< Planned duration in milliseconds. */
    q16_16 progress_ms;          /**< Progress in milliseconds. */
} planner_segment_t;

/**
 * @brief Planner ring buffer container.
 */
typedef struct
{
    planner_limits_t limits;            /**< Global motion limits. */
    planner_segment_t segments[16];     /**< Circular buffer of segments. */
    size_t head;                        /**< Index of first valid entry. */
    size_t tail;                        /**< Index past the last valid entry. */
    bool time_scaled;                   /**< true if time-scaling applied. */
} planner_t;

/**
 * @brief Initialize the planner with provided limits.
 *
 * @param planner Planner context.
 * @param limits Motion limits.
 */
void planner_init(planner_t *planner, const planner_limits_t *limits);

/**
 * @brief Enqueue a linear segment.
 *
 * @param planner Planner context.
 * @param start Start point.
 * @param end End point.
 * @return true on success, false if the queue is full.
 */
bool planner_enqueue_linear(planner_t *planner,
                            const vec3_q16 *start,
                            const vec3_q16 *end);

/**
 * @brief Enqueue a spline segment.
 *
 * @param planner Planner context.
 * @param spline Precomputed spline plan.
 * @return true on success, false if the queue is full.
 */
bool planner_enqueue_spline(planner_t *planner, const spl_plan_t *spline);

/**
 * @brief Advance the planner by one control period.
 *
 * @param planner Planner context.
 * @param period_ms Control period in milliseconds.
 * @param out Next target position in Cartesian coordinates.
 * @return true if a valid set-point is produced.
 */
bool planner_step(planner_t *planner, q16_16 period_ms, vec3_q16 *out);

/**
 * @brief Query remaining buffered duration.
 *
 * @param planner Planner context.
 * @return Remaining time in milliseconds.
 */
q16_16 planner_buffer_time(const planner_t *planner);

#endif /* PLANNER_PLANNER_H */
