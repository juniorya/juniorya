/**
 * @file visualizer3d.h
 * @brief Lightweight 3D visualisation utilities for simulator output.
 */

#ifndef VISUALIZER3D_H
#define VISUALIZER3D_H

#include <stdbool.h>

#include "planner/splines/splines.h"
#include "utils/vec3.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Camera description for perspective projection.
 */
typedef struct
{
    vec3_q16 position; /**< Camera position in workspace coordinates. */
    vec3_q16 target;   /**< Look-at target point. */
    q16_16 fov_deg;    /**< Field-of-view angle in degrees (Q16.16). */
} visualizer3d_camera;

/**
 * @brief Export trajectory samples as Wavefront OBJ polyline.
 * @param traj Time scaled trajectory to render.
 * @param path Output file path.
 * @return True on success.
 */
bool visualizer3d_export_obj(const time_scaled_traj_t *traj, const char *path);

/**
 * @brief Render trajectory into a PPM image using simple perspective projection.
 * @param traj Time scaled trajectory.
 * @param cam Camera parameters.
 * @param path Output image path.
 * @return True on success.
 */
bool visualizer3d_render_ppm(const time_scaled_traj_t *traj,
                             const visualizer3d_camera *cam,
                             const char *path);

#ifdef __cplusplus
}
#endif

#endif
