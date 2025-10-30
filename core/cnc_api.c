/**
 * @file cnc_api.c
 * @brief Implementation of GUI facing helpers built on top of the spline core.
 */

#include "core/include/cnc_api.h"

#include "utils/vec3.h"

#include <string.h>

void cnc_api_waypoints_default(cnc_waypoint_buffer *buffer)
{
    if (buffer == NULL)
    {
        return;
    }
    buffer->count = 4U;
    buffer->points[0] = vec3_from_float(-50.0f, 0.0f, -50.0f);
    buffer->points[1] = vec3_from_float(0.0f, 50.0f, -40.0f);
    buffer->points[2] = vec3_from_float(50.0f, 0.0f, -30.0f);
    buffer->points[3] = vec3_from_float(0.0f, -50.0f, -20.0f);
}

bool cnc_api_build_preview(const cnc_waypoint_buffer *buffer,
                           spl_type_t type,
                           q16_16 period_ms,
                           cnc_preview_t *preview)
{
    if ((buffer == NULL) || (preview == NULL))
    {
        return false;
    }
    if ((buffer->count < 2U) || (buffer->count > CNC_API_MAX_WAYPOINTS))
    {
        return false;
    }
    memset(preview, 0, sizeof(*preview));
    preview->plan.max_vel = q16_from_int(200);
    preview->plan.max_acc = q16_from_int(1000);
    preview->plan.max_jerk = q16_from_int(5000);
    preview->plan.control_count = buffer->count;
    for (size_t i = 0; i < buffer->count; ++i)
    {
        preview->plan.control[i] = buffer->points[i];
    }
    if (type == SPL_BRACHISTOCHRONE)
    {
        if (buffer->count != 2U)
        {
            return false;
        }
        if (!spl_make_brachistochrone(&buffer->points[0], &buffer->points[1], &preview->plan))
        {
            return false;
        }
    }
    else
    {
        if (!spl_make_from_waypoints(buffer->points,
                                     buffer->count,
                                     NULL,
                                     NULL,
                                     &preview->plan))
        {
            return false;
        }
        preview->plan.type = type;
    }
    if (!spl_time_scale(&preview->plan, period_ms, &preview->samples))
    {
        return false;
    }
    return true;
}

size_t cnc_api_preview_as_float(const cnc_preview_t *preview,
                                size_t capacity,
                                float *positions,
                                float *velocities)
{
    if ((preview == NULL) || (positions == NULL) || (capacity == 0U))
    {
        return 0U;
    }
    size_t count = preview->samples.count;
    if (count > capacity)
    {
        count = capacity;
    }
    for (size_t i = 0; i < count; ++i)
    {
        positions[i * 3U + 0U] = q16_to_float(preview->samples.position[i].x);
        positions[i * 3U + 1U] = q16_to_float(preview->samples.position[i].y);
        positions[i * 3U + 2U] = q16_to_float(preview->samples.position[i].z);
        if (velocities != NULL)
        {
            velocities[i * 3U + 0U] = q16_to_float(preview->samples.velocity[i].x);
            velocities[i * 3U + 1U] = q16_to_float(preview->samples.velocity[i].y);
            velocities[i * 3U + 2U] = q16_to_float(preview->samples.velocity[i].z);
        }
    }
    return count;
}
