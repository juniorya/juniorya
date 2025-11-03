#ifndef CALIB_H
#define CALIB_H

#include <stdbool.h>
#include <stddef.h>

#include "kinematics/delta.h"
#include "storage/storage.h"

/** \brief Calibration context. */
typedef struct
{
    delta_cfg_t config; /**< Active configuration. */
} calib_context;

/** \brief Initialise calibration context. */
bool calib_init(calib_context *ctx, const delta_cfg_t *defaults);

/** \brief Load calibration data from storage. */
bool calib_load(calib_context *ctx);

/** \brief Persist calibration data. */
bool calib_save(const calib_context *ctx);

/** \brief Apply homing offsets. */
bool calib_home(calib_context *ctx, const q16_16 offsets[3]);

/** \brief Apply zero offset to axis. */
bool calib_zero(calib_context *ctx, size_t axis, q16_16 offset);

/** \brief Apply scale factor to axis. */
bool calib_scale(calib_context *ctx, size_t axis, q16_16 scale);

#endif
