/**
 * @file ide_project.h
 * @brief High-level project description and tooling for the desktop IDE.
 */

#ifndef IDE_PROJECT_H
#define IDE_PROJECT_H

#include <stdbool.h>
#include <stddef.h>

#include "planner/splines/splines.h"
#include "sim/visualizer3d.h"
#include "sim/configurator.h"
#include "ml/tinyml_optimizer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Maximum number of waypoints supported by the IDE loader.
 */
#define IDE_MAX_WAYPOINTS 32U

/**
 * @brief IDE project descriptor used by the graphical authoring tool.
 */
typedef struct
{
    char name[64];                 /**< Human readable project name. */
    spl_type_t requested_type;     /**< Curve family requested by the designer. */
    q16_16 period_ms;              /**< Sampling period for time scaling. */
    vec3_q16 waypoints[IDE_MAX_WAYPOINTS]; /**< Waypoints declared in the project file. */
    size_t waypoint_count;         /**< Count of valid waypoints. */
    q16_16 limit_velocity;         /**< Velocity limit requested by the project. */
    q16_16 limit_acceleration;     /**< Acceleration constraint. */
    q16_16 limit_jerk;             /**< Jerk constraint. */
    spl_plan_t plan;               /**< Generated spline plan. */
    time_scaled_traj_t trajectory; /**< Resulting time scaled samples. */
    tinyml_network optimiser;      /**< Embedded TinyML optimiser instance. */
    bool plan_ready;               /**< True when plan and trajectory are valid. */
} ide_project;

/**
 * @brief Load project description from a text file.
 *
 * The parser accepts simple key-value directives:
 * - `NAME <string>`
 * - `TYPE <BEZIER3|BSPLINE3|QUINTIC|NURBS3>`
 * - `PERIOD_MS <float>`
 * - `LIMITS <vel_mm_s> <acc_mm_s2> <jerk_mm_s3>`
 * - `WAYPOINT <x> <y> <z>`
 * Comments starting with `#` are ignored.
 *
 * @param path Path to the project file.
 * @param project Output project structure.
 * @return True on success.
 */
bool ide_project_load(const char *path, ide_project *project);

/**
 * @brief Generate spline and time scaling for the given project.
 *
 * The routine calls the TinyML optimiser to adapt velocity and acceleration
 * limits and stores the final sampled trajectory within the project.
 *
 * @param project Project to process.
 * @return True when the plan and trajectory were generated.
 */
bool ide_project_generate(ide_project *project);

/**
 * @brief Export artefacts for offline visualisation.
 *
 * The function creates a 2D configuration preview, a 3D OBJ/PPM render
 * and a textual report summarising timing information.
 *
 * @param project Project descriptor with a valid trajectory.
 * @param directory Output directory path.
 * @return True on success.
 */
bool ide_project_export(const ide_project *project, const char *directory);

#ifdef __cplusplus
}
#endif

#endif
