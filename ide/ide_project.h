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
#include "ml/tinyml_configurator.h"
#include "ide/st_compiler.h"
#include "kinematics/delta_kinematics.h"
#include "vision/vision.h"
#include "ethcat/ethcat.h"

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
    tinyml_profile optimiser_profile; /**< Profile used to tune optimiser. */
    bool optimiser_profile_loaded; /**< Profile loaded from external file. */
    char optimiser_profile_path[128]; /**< Path to TinyML profile file. */
    char optimiser_profile_desc[TINYML_PROFILE_DESC_LEN]; /**< Profile summary string. */
    bool plan_ready;               /**< True when plan and trajectory are valid. */
    vision_config vision;          /**< Vision configuration snapshot. */
    char drive_xml_path[128];      /**< Optional KEBA XML descriptor path. */
    bool drive_xml_loaded;         /**< True when XML import succeeded. */
    ethcat_drive_descriptor drive_info; /**< First drive descriptor parsed from XML. */
    char st_program_path[128];     /**< Structured Text program path. */
    bool st_program_present;       /**< True when ST source was parsed. */
    stc_compilation st_program;    /**< Compiled ST artefacts. */
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
 * - `VISION_TOLERANCE <mm>`
 * - `KEBA_XML <path>`
 * - `TINYML_PROFILE <path>`
 * - `ST_PROGRAM <path>`
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
 * limits and stores the final sampled trajectory within the project. When a
 * profile is provided, its gains adjust the optimiser weights before
 * evaluation.
 *
 * @param project Project to process.
 * @return True when the plan and trajectory were generated.
 */
bool ide_project_generate(ide_project *project);

/**
 * @brief Export artefacts for offline visualisation.
 *
 * The function creates a 2D configuration preview, a 3D OBJ/PPM render
 * and a textual report summarising timing information. Structured Text code
 * is translated to C and stored alongside metadata for syntax highlighting
 * and auto-completion.
 *
 * @param project Project descriptor with a valid trajectory.
 * @param directory Output directory path.
 * @return True on success.
 */
bool ide_project_export(const ide_project *project, const char *directory);

/**
 * @brief Export low-level debug trace including joint angles.
 *
 * The debug artefact enumerates each sampled waypoint, the corresponding
 * inverse-kinematics joint positions and the TinyML optimiser multiplier.
 *
 * @param project Project descriptor.
 * @param directory Output directory path.
 * @return True on success.
 */
bool ide_project_export_debug(const ide_project *project, const char *directory);

#ifdef __cplusplus
}
#endif

#endif
