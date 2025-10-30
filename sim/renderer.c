/**
 * @file renderer.c
 * @brief Software renderer implementation for simulator output.
 */

#include "renderer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool renderer_init(renderer_canvas *canvas, size_t width, size_t height)
{
    canvas->width = width;
    canvas->height = height;
    canvas->px = (unsigned char *)calloc(width * height * 3U, sizeof(unsigned char));
    return canvas->px != NULL;
}

void renderer_clear(renderer_canvas *canvas)
{
    memset(canvas->px, 0, canvas->width * canvas->height * 3U);
}

void renderer_plot(renderer_canvas *canvas, float x_mm, float y_mm)
{
    float scale = 1.0f;
    int cx = (int)(canvas->width / 2);
    int cy = (int)(canvas->height / 2);
    int px = cx + (int)(x_mm * scale);
    int py = cy - (int)(y_mm * scale);
    if (px < 0 || py < 0 || (size_t)px >= canvas->width || (size_t)py >= canvas->height)
    {
        return;
    }
    size_t index = ((size_t)py * canvas->width + (size_t)px) * 3U;
    canvas->px[index + 0] = 255U;
    canvas->px[index + 1] = 200U;
    canvas->px[index + 2] = 0U;
}

bool renderer_save_ppm(const renderer_canvas *canvas, const char *path)
{
    FILE *f = fopen(path, "w");
    if (f == NULL)
    {
        return false;
    }
    fprintf(f, "P3\n%zu %zu\n255\n", canvas->width, canvas->height);
    for (size_t y = 0; y < canvas->height; ++y)
    {
        for (size_t x = 0; x < canvas->width; ++x)
        {
            size_t idx = (y * canvas->width + x) * 3U;
            fprintf(f, "%u %u %u ", canvas->px[idx], canvas->px[idx + 1], canvas->px[idx + 2]);
        }
        fprintf(f, "\n");
    }
    fclose(f);
    return true;
}

bool renderer_save_csv(const time_scaled_traj_t *traj, const char *path)
{
    FILE *f = fopen(path, "w");
    if (f == NULL)
    {
        return false;
    }
    fprintf(f, "index,x_mm,y_mm,z_mm\n");
    for (size_t i = 0; i < traj->count; ++i)
    {
        fprintf(f, "%zu,%.4f,%.4f,%.4f\n", i,
                q16_to_float(traj->position[i].x),
                q16_to_float(traj->position[i].y),
                q16_to_float(traj->position[i].z));
    }
    fclose(f);
    return true;
}

void renderer_deinit(renderer_canvas *canvas)
{
    free(canvas->px);
    canvas->px = NULL;
}
