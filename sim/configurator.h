/**
 * @file configurator.h
 * @brief Graphical configurator facade for desktop tooling.
 */

#ifndef CONFIGURATOR_H
#define CONFIGURATOR_H

#include <stdbool.h>

#include "kinematics/delta_kinematics.h"
#include "planner/planner.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Combined configuration snapshot used by the GUI configurator.
 */
typedef struct
{
    planner_config planner; /**< Planner parameters mirrored into UI. */
    delta_cfg_t delta;       /**< Delta robot geometry and calibration. */
} configurator_profile;

/**
 * @brief Initialise profile with safe defaults.
 * @param profile Profile instance.
 */
void configurator_default_profile(configurator_profile *profile);

/**
 * @brief Render a static UI representation into a PPM image.
 * @param profile Profile to visualise.
 * @param ppm_path Output image file path.
 * @return True on success.
 */
bool configurator_render_ui(const configurator_profile *profile, const char *ppm_path);

/**
 * @brief Export profile into JSON suitable for loading at runtime.
 * @param profile Profile data.
 * @param path Output JSON file path.
 * @return True on success.
 */
bool configurator_export_profile(const configurator_profile *profile, const char *path);

#ifdef __cplusplus
}
#endif

#endif
