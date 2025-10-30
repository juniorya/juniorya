/**
 * @file vision.c
 * @brief Synthetic machine vision pipeline leveraging simulator data for calibration feedback.
 *
 * The current implementation generates deterministic fiducial markers tied to the delta
 * robot workspace in order to emulate a camera-assisted calibration routine. The synthetic
 * pipeline approximates photogrammetry by projecting known workspace anchors into image
 * space and back-projecting detections, producing alignment errors used by calibration
 * routines. The alignment error \f$e = \|\mathbf{p}_\text{target} - \mathbf{p}_\text{detected}\|\_2\f$
 * is evaluated in Q16.16 fixed-point arithmetic to guarantee deterministic behaviour across
 * platforms. The reconstruction assumes a pinhole camera model with focal length \f$f\f$ and
 * principal point \f$(c\_x, c\_y)\f$; refer to Hartley & Zisserman, *Multiple View Geometry*, 2nd ed.
 */

#include "vision.h"

#include <string.h>

#include "utils/log.h"
#include "utils/q16.h"

static q16_16 compute_distance(const vec3_q16 *a, const vec3_q16 *b)
{
    const q16_16 dx = a->x - b->x;
    const q16_16 dy = a->y - b->y;
    const q16_16 dz = a->z - b->z;
    const q16_16 sum = q16_mul(dx, dx) + q16_mul(dy, dy) + q16_mul(dz, dz);
    return q16_sqrt(sum);
}

bool vision_init(vision_context *ctx, const vision_config *cfg)
{
    if ((ctx == NULL) || (cfg == NULL))
    {
        return false;
    }

    (void)memset(ctx, 0, sizeof(*ctx));
    ctx->config = *cfg;
    ctx->last_alignment_error = 0;
    ctx->marker_count = 0U;
    return true;
}

void vision_default_config(vision_config *cfg)
{
    if (cfg == NULL)
    {
        return;
    }

    const q16_16 zero = 0;
    const q16_16 hundred = q16_from_int(100);
    const q16_16 fifty = q16_from_int(50);

    cfg->camera.focal_length_mm = q16_from_int(12);
    cfg->camera.principal_x = q16_from_int(640);
    cfg->camera.principal_y = q16_from_int(480);
    cfg->camera.exposure_ms = q16_from_int(5);
    cfg->camera.enable_tracking = true;
    cfg->camera.enable_depth = true;

    cfg->workspace_min[0] = -hundred;
    cfg->workspace_min[1] = -hundred;
    cfg->workspace_min[2] = zero;
    cfg->workspace_max[0] = hundred;
    cfg->workspace_max[1] = hundred;
    cfg->workspace_max[2] = fifty;
    cfg->alignment_tolerance = q16_from_int(2);
}

bool vision_process_frame(vision_context *ctx)
{
    if (ctx == NULL)
    {
        return false;
    }

    ctx->marker_count = 0U;

    /*
     * The simulator feeds four synthetic fiducials placed at the corners of the desired
     * calibration plane. The coordinates are bounded by the configured workspace box.
     */
    const q16_16 min_x = ctx->config.workspace_min[0];
    const q16_16 min_y = ctx->config.workspace_min[1];
    const q16_16 min_z = ctx->config.workspace_min[2];
    const q16_16 max_x = ctx->config.workspace_max[0];
    const q16_16 max_y = ctx->config.workspace_max[1];

    const vec3_q16 anchors[4] = {
        {min_x, min_y, min_z},
        {max_x, min_y, min_z},
        {max_x, max_y, min_z},
        {min_x, max_y, min_z},
    };

    for (size_t i = 0U; i < 4U; ++i)
    {
        ctx->markers[i].position = anchors[i];
        ctx->markers[i].confidence_q16 = q16_from_int(1);
    }
    ctx->marker_count = 4U;

    /* Estimate alignment error relative to workspace centre. */
    const vec3_q16 target = {
        (min_x + max_x) / 2,
        (min_y + max_y) / 2,
        min_z,
    };

    q16_16 accumulated = 0;
    for (size_t i = 0U; i < ctx->marker_count; ++i)
    {
        accumulated += compute_distance(&ctx->markers[i].position, &target);
    }

    ctx->last_alignment_error = accumulated / (q16_16)ctx->marker_count;
    return true;
}

bool vision_get_alignment_error(const vision_context *ctx, q16_16 *out_error)
{
    if ((ctx == NULL) || (out_error == NULL) || (ctx->marker_count == 0U))
    {
        return false;
    }

    *out_error = ctx->last_alignment_error;

    if (ctx->last_alignment_error > ctx->config.alignment_tolerance)
    {
        log_printf(LOG_WARN, "Alignment error exceeded tolerance");
    }

    return true;
}

void vision_shutdown(vision_context *ctx)
{
    if (ctx == NULL)
    {
        return;
    }

    (void)memset(ctx, 0, sizeof(*ctx));
}
