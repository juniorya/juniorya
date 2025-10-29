/**
 * @file visualizer3d.c
 * @brief Minimal 3D visualisation back-end generating OBJ meshes and PPM renders.
 */

#include "visualizer3d.h"

#include <math.h>
#include <stdio.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static float q16_to_f(q16_16 v)
{
    return q16_to_float(v);
}

static void normalise(vec3_q16 *v)
{
    q16_16 len = vec3_norm(*v);
    bool ok = true;
    if (len == 0)
    {
        return;
    }
    q16_16 inv = q16_div(q16_from_int(1), len, &ok);
    if (!ok)
    {
        return;
    }
    *v = vec3_scale(*v, inv);
}

bool visualizer3d_export_obj(const time_scaled_traj_t *traj, const char *path)
{
    if ((traj == NULL) || (path == NULL))
    {
        return false;
    }
    FILE *fp = fopen(path, "w");
    if (fp == NULL)
    {
        return false;
    }
    fprintf(fp, "# Delta CNC trajectory export\n");
    for (size_t i = 0; i < traj->count; ++i)
    {
        fprintf(fp, "v %.6f %.6f %.6f\n",
                q16_to_f(traj->position[i].x),
                q16_to_f(traj->position[i].y),
                q16_to_f(traj->position[i].z));
    }
    fprintf(fp, "o trajectory\n");
    for (size_t i = 1; i < traj->count; ++i)
    {
        fprintf(fp, "l %zu %zu\n", i, i + 1);
    }
    fclose(fp);
    return true;
}

static void compute_basis(const visualizer3d_camera *cam, vec3_q16 *forward, vec3_q16 *right, vec3_q16 *up)
{
    *forward = vec3_sub(cam->target, cam->position);
    normalise(forward);
    *up = vec3_from_float(0.0f, 0.0f, 1.0f);
    *right = vec3_cross(*forward, *up);
    normalise(right);
    *up = vec3_cross(*right, *forward);
    normalise(up);
}

bool visualizer3d_render_ppm(const time_scaled_traj_t *traj,
                             const visualizer3d_camera *cam,
                             const char *path)
{
    if ((traj == NULL) || (cam == NULL) || (path == NULL))
    {
        return false;
    }
    FILE *fp = fopen(path, "w");
    if (fp == NULL)
    {
        return false;
    }
    const int width = 640;
    const int height = 480;
    fprintf(fp, "P3\n%d %d\n255\n", width, height);

    vec3_q16 forward;
    vec3_q16 right;
    vec3_q16 up;
    compute_basis(cam, &forward, &right, &up);
    float fov = q16_to_f(cam->fov_deg) * (float)M_PI / 180.0f;
    float aspect = (float)width / (float)height;
    float scale = tanf(fov * 0.5f);

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            int color = 16;
            for (size_t i = 0; i < traj->count; ++i)
            {
                vec3_q16 pt = traj->position[i];
                vec3_q16 rel = vec3_sub(pt, cam->position);
                float rx = q16_to_f(vec3_dot(rel, right));
                float ry = q16_to_f(vec3_dot(rel, up));
                float rz = q16_to_f(vec3_dot(rel, forward));
                if (rz <= 0.0f)
                {
                    continue;
                }
                float px = (rx / (rz * scale)) * aspect;
                float py = (ry / (rz * scale));
                int sx = (int)((px * 0.5f + 0.5f) * (float)width);
                int sy = (int)((-py * 0.5f + 0.5f) * (float)height);
                if ((sx == x) && (sy == y))
                {
                    color = 255;
                    break;
                }
            }
            fprintf(fp, "%d %d %d ", color, color, color);
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
    return true;
}
