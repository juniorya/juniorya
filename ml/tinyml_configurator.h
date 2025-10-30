/**
 * @file tinyml_configurator.h
 * @brief Configuration helpers for tuning the embedded TinyML optimiser.
 */

#ifndef TINYML_CONFIGURATOR_H
#define TINYML_CONFIGURATOR_H

#include <stdbool.h>
#include <stddef.h>

#include "tinyml_optimizer.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Maximum length of textual description emitted by configurator. */
#define TINYML_PROFILE_DESC_LEN 128U

/**
 * @brief User controllable parameters for the neural optimiser.
 */
typedef struct
{
    q16_16 curvature_gain;      /**< Gain applied to curvature feature. */
    q16_16 acceleration_gain;   /**< Gain applied to acceleration feature. */
    q16_16 velocity_floor;      /**< Minimum allowed velocity multiplier. */
    q16_16 velocity_ceiling;    /**< Maximum allowed velocity multiplier. */
    bool clamp_output;          /**< Clamp optimiser output to floor/ceiling. */
} tinyml_profile;

/**
 * @brief Populate profile with conservative defaults.
 * @param profile Profile instance.
 */
void tinyml_profile_default(tinyml_profile *profile);

/**
 * @brief Load profile parameters from configuration file.
 *
 * The file uses a simple `key = value` format with recognised keys:
 * `CURVATURE_GAIN`, `ACCELERATION_GAIN`, `VELOCITY_FLOOR`,
 * `VELOCITY_CEILING` and `CLAMP_OUTPUT` (0/1).
 *
 * @param path Filesystem path to profile.
 * @param profile Profile instance to populate.
 * @return True when parsing succeeds.
 */
bool tinyml_profile_load(const char *path, tinyml_profile *profile);

/**
 * @brief Apply profile gains to optimiser coefficients.
 * @param profile Profile configuration.
 * @param net Neural network to modify.
 * @return True when application succeeds.
 */
bool tinyml_profile_apply(const tinyml_profile *profile, tinyml_network *net);

/**
 * @brief Produce textual description of active profile for IDE display.
 * @param profile Profile configuration.
 * @param buffer Destination buffer.
 * @param capacity Length of destination buffer in bytes.
 * @return True if description was truncated safely.
 */
bool tinyml_profile_describe(const tinyml_profile *profile, char *buffer, size_t capacity);

#ifdef __cplusplus
}
#endif

#endif
