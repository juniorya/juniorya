/**
 * @file calib.h
 * @brief Calibration routines for homing and geometry correction.
 */

#ifndef CALIB_H
#define CALIB_H

#include <stdbool.h>

#include "kinematics/delta_kinematics.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Calibration context. */
typedef struct
{
    delta_cfg_t config; /**< Robot configuration subject to calibration. */
} calib_context;

/**
 * @brief Initialise calibration context.
 * @param ctx Calibration context.
 * @param cfg Baseline configuration.
 */
void calib_init(calib_context *ctx, const delta_cfg_t *cfg);

/**
 * @brief Perform homing procedure and update offsets.
 * @param ctx Calibration context.
 * @param offsets Encoder offsets.
 */
void calib_perform_homing(calib_context *ctx, const q16_16 offsets[3]);

/**
 * @brief Apply scale calibration per axis.
 * @param ctx Calibration context.
 * @param scales New scale factors.
 */
void calib_apply_scale(calib_context *ctx, const q16_16 scales[3]);

#ifdef __cplusplus
}
#endif

#endif
