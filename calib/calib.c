/**
 * @file calib.c
 * @brief Calibration helper routines.
 */

#include "calib.h"

#include <string.h>

void calib_init(calib_context *ctx, const delta_cfg_t *cfg)
{
    ctx->config = *cfg;
}

void calib_perform_homing(calib_context *ctx, const q16_16 offsets[3])
{
    for (int i = 0; i < 3; ++i)
    {
        ctx->config.calib_offsets[i] = offsets[i];
    }
}

void calib_apply_scale(calib_context *ctx, const q16_16 scales[3])
{
    for (int i = 0; i < 3; ++i)
    {
        ctx->config.scale_per_axis[i] = scales[i];
    }
}
