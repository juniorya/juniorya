#include "calib/calib.h"

#include <string.h>

#define STORAGE_KEY_CALIB "calibration"

typedef struct
{
    q16_16 offsets[3];
    q16_16 scales[3];
} calib_blob;

bool calib_init(calib_context *ctx, const delta_cfg_t *defaults)
{
    if ((ctx == NULL) || (defaults == NULL))
    {
        return false;
    }
    ctx->config = *defaults;
    return true;
}

bool calib_load(calib_context *ctx)
{
    if (ctx == NULL)
    {
        return false;
    }
    calib_blob blob;
    if (!storage_read(STORAGE_KEY_CALIB, &blob, sizeof(blob)))
    {
        return false;
    }
    for (size_t i = 0; i < 3; ++i)
    {
        ctx->config.calib_offsets[i] = blob.offsets[i];
        ctx->config.scale_per_axis[i] = blob.scales[i];
    }
    return true;
}

bool calib_save(const calib_context *ctx)
{
    if (ctx == NULL)
    {
        return false;
    }
    calib_blob blob;
    for (size_t i = 0; i < 3; ++i)
    {
        blob.offsets[i] = ctx->config.calib_offsets[i];
        blob.scales[i] = ctx->config.scale_per_axis[i];
    }
    return storage_write(STORAGE_KEY_CALIB, &blob, sizeof(blob));
}

bool calib_home(calib_context *ctx, const q16_16 offsets[3])
{
    if ((ctx == NULL) || (offsets == NULL))
    {
        return false;
    }
    for (size_t i = 0; i < 3; ++i)
    {
        ctx->config.calib_offsets[i] = offsets[i];
    }
    return true;
}

bool calib_zero(calib_context *ctx, size_t axis, q16_16 offset)
{
    if ((ctx == NULL) || (axis >= 3U))
    {
        return false;
    }
    ctx->config.calib_offsets[axis] = offset;
    return true;
}

bool calib_scale(calib_context *ctx, size_t axis, q16_16 scale)
{
    if ((ctx == NULL) || (axis >= 3U))
    {
        return false;
    }
    ctx->config.scale_per_axis[axis] = scale;
    return true;
}
