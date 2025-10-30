/**
 * @file configurator.c
 * @brief Offline graphical configurator rendering implementation.
 */

#include "configurator.h"

#include <stdio.h>

static float q16_to_f(q16_16 v)
{
    return q16_to_float(v);
}

void configurator_default_profile(configurator_profile *profile)
{
    planner_default_config(&profile->planner);
    delta_default_config(&profile->delta);
    vision_default_config(&profile->vision);
}

bool configurator_render_ui(const configurator_profile *profile, const char *ppm_path)
{
    if ((profile == NULL) || (ppm_path == NULL))
    {
        return false;
    }
    FILE *fp = fopen(ppm_path, "w");
    if (fp == NULL)
    {
        return false;
    }
    const int width = 480;
    const int height = 320;
    fprintf(fp, "P3\n%d %d\n255\n", width, height);
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            int r = 32;
            int g = 32;
            int b = 32;
            if (x < width / 2)
            {
                float ratio = (float)y / (float)height;
                float vel = q16_to_f(profile->planner.max_velocity) / 400.0f;
                if (ratio < vel)
                {
                    r = 0;
                    g = 180;
                    b = 0;
                }
            }
            else
            {
                float ratio = (float)y / (float)height;
                float acc = q16_to_f(profile->planner.max_acceleration) / 2000.0f;
                if (ratio < acc)
                {
                    r = 180;
                    g = 0;
                    b = 0;
                }
                else
                {
                    float tol = q16_to_f(profile->vision.alignment_tolerance) / 5.0f;
                    if (ratio < tol)
                    {
                        r = 0;
                        g = 0;
                        b = 180;
                    }
                }
            }
            fprintf(fp, "%d %d %d ", r, g, b);
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
    return true;
}

bool configurator_export_profile(const configurator_profile *profile, const char *path)
{
    if ((profile == NULL) || (path == NULL))
    {
        return false;
    }
    FILE *fp = fopen(path, "w");
    if (fp == NULL)
    {
        return false;
    }
    fprintf(fp, "{\n");
    fprintf(fp, "  \"planner\": {\n");
    fprintf(fp, "    \"max_velocity\": %.3f,\n", q16_to_f(profile->planner.max_velocity));
    fprintf(fp, "    \"max_acceleration\": %.3f,\n", q16_to_f(profile->planner.max_acceleration));
    fprintf(fp, "    \"max_jerk\": %.3f\n", q16_to_f(profile->planner.max_jerk));
    fprintf(fp, "  },\n");
    fprintf(fp, "  \"delta\": {\n");
    fprintf(fp, "    \"R_base\": %.3f,\n", q16_to_f(profile->delta.R_base));
    fprintf(fp, "    \"r_eff\": %.3f,\n", q16_to_f(profile->delta.r_eff));
    fprintf(fp, "    \"L_upper\": %.3f,\n", q16_to_f(profile->delta.L_upper));
    fprintf(fp, "    \"L_lower\": %.3f,\n", q16_to_f(profile->delta.L_lower));
    fprintf(fp, "    \"z_offset\": %.3f\n", q16_to_f(profile->delta.z_offset));
    fprintf(fp, "  },\n");
    fprintf(fp, "  \"vision\": {\n");
    fprintf(fp, "    \"focal_length_mm\": %.3f,\n", q16_to_f(profile->vision.camera.focal_length_mm));
    fprintf(fp, "    \"principal_x\": %.3f,\n", q16_to_f(profile->vision.camera.principal_x));
    fprintf(fp, "    \"principal_y\": %.3f,\n", q16_to_f(profile->vision.camera.principal_y));
    fprintf(fp, "    \"exposure_ms\": %.3f,\n", q16_to_f(profile->vision.camera.exposure_ms));
    fprintf(fp, "    \"alignment_tolerance\": %.3f\n", q16_to_f(profile->vision.alignment_tolerance));
    fprintf(fp, "  }\n");
    fprintf(fp, "}\n");
    fclose(fp);
    return true;
}
