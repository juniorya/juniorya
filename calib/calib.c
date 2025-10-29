/**
 * @file calib.c
 * @brief Calibration procedures.
 */
#include "calib.h"

#include <string.h>

void calib_init(calib_context_t *ctx,
                const delta_cfg_t *defaults,
                storage_t *storage)
{
    ctx->config = *defaults;
    ctx->storage = storage;
}

bool calib_run_home(calib_context_t *ctx)
{
    (void)ctx;
    return true;
}

bool calib_zero_axis(calib_context_t *ctx, size_t axis)
{
    if (axis >= 3U)
    {
        return false;
    }
    ctx->config.calib_offsets[axis] = 0;
    return true;
}

bool calib_save(calib_context_t *ctx)
{
    return storage_put(ctx->storage,
                       "calib",
                       (const uint8_t *)&ctx->config,
                       sizeof(ctx->config)) &&
           storage_save(ctx->storage);
}

bool calib_load(calib_context_t *ctx)
{
    size_t len = sizeof(ctx->config);
    if (!storage_get(ctx->storage,
                     "calib",
                     (uint8_t *)&ctx->config,
                     &len))
    {
        return false;
    }
    return storage_load(ctx->storage);
}
