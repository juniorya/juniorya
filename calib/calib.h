/**
 * @file calib.h
 * @brief Calibration routines for the delta robot.
 */
#ifndef CALIB_CALIB_H
#define CALIB_CALIB_H

#include <stdbool.h>
#include <stddef.h>

#include "kinematics/delta_kinematics.h"
#include "storage/storage.h"

/**
 * @brief Calibration context containing kinematic parameters.
 */
typedef struct
{
    delta_cfg_t config; /**< Active configuration. */
    storage_t *storage; /**< Storage backend. */
} calib_context_t;

/**
 * @brief Initialize calibration context.
 *
 * @param ctx Calibration context.
 * @param defaults Default configuration.
 * @param storage Persistent storage backend.
 */
void calib_init(calib_context_t *ctx,
                const delta_cfg_t *defaults,
                storage_t *storage);

/**
 * @brief Execute homing sequence.
 *
 * @param ctx Calibration context.
 * @return true on success.
 */
bool calib_run_home(calib_context_t *ctx);

/**
 * @brief Zero a specific axis.
 *
 * @param ctx Calibration context.
 * @param axis Axis index.
 * @return true on success.
 */
bool calib_zero_axis(calib_context_t *ctx, size_t axis);

/**
 * @brief Save calibration to storage.
 *
 * @param ctx Calibration context.
 * @return true on success.
 */
bool calib_save(calib_context_t *ctx);

/**
 * @brief Load calibration from storage.
 *
 * @param ctx Calibration context.
 * @return true on success.
 */
bool calib_load(calib_context_t *ctx);

#endif /* CALIB_CALIB_H */
