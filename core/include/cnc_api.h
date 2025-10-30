/**
 * @file cnc_api.h
 * @brief Public C interface exposing trajectory and kinematics services for GUI front-ends.
 */

#ifndef CNC_API_H
#define CNC_API_H

#include <stddef.h>
#include <stdbool.h>

#include "planner/splines/splines.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \brief Maximum number of waypoints accepted by the GUI helper API. */
#define CNC_API_MAX_WAYPOINTS 32U

/** \brief Container holding a mutable list of spline waypoints. */
typedef struct
{
    size_t count;               /**< Number of valid entries in @ref points. */
    vec3_q16 points[CNC_API_MAX_WAYPOINTS]; /**< Waypoint coordinates in machine space. */
} cnc_waypoint_buffer;

/** \brief Preview object bundling spline plan and time-scaled samples. */
typedef struct
{
    spl_plan_t plan;            /**< Underlying spline definition. */
    time_scaled_traj_t samples; /**< Discrete trajectory derived from @ref plan. */
} cnc_preview_t;

/**
 * @brief Initialise waypoint buffer with default demo content.
 *
 * The helper fills the buffer with a four-point pattern identical to the
 * trajectory showcased in the legacy console simulator. GUI callers can use
 * this as a quick-start scene when creating a new project.
 *
 * @param buffer Mutable waypoint collection.
 */
void cnc_api_waypoints_default(cnc_waypoint_buffer *buffer);

/**
 * @brief Build spline preview using Shin–McKay time scaling.
 *
 * The routine expands the supplied control points into a spline plan, applies
 * jerk-limited time scaling and stores the resulting samples inside
 * @p preview. The buffer is fully self-contained; no heap allocation is
 * performed which makes the API deterministic for real-time previews.
 *
 * @param buffer Input waypoints.
 * @param type Desired spline type.
 * @param period_ms Sampling period in milliseconds.
 * @param preview Output preview (must not be NULL).
 * @return True on success, false if arguments are invalid or cannot produce a spline.
 */
bool cnc_api_build_preview(const cnc_waypoint_buffer *buffer,
                           spl_type_t type,
                           q16_16 period_ms,
                           cnc_preview_t *preview);

/**
 * @brief Sample preview trajectory into float arrays for rendering.
 *
 * The helper converts fixed-point vectors into float triplets compatible with
 * the OpenGL renderer. The caller owns @p positions and @p velocities buffers
 * and must ensure they contain at least @p capacity elements. The function
 * returns the number of filled samples.
 *
 * @param preview Prepared preview from @ref cnc_api_build_preview.
 * @param capacity Maximum number of elements the output buffers can hold.
 * @param positions Destination array of size @p capacity * 3.
 * @param velocities Destination array of size @p capacity * 3 (may be NULL to skip).
 * @return Number of samples written, zero on error.
 */
size_t cnc_api_preview_as_float(const cnc_preview_t *preview,
                                size_t capacity,
                                float *positions,
                                float *velocities);

#ifdef __cplusplus
}
#endif

#endif /* CNC_API_H */
