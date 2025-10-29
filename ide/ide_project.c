/**
 * @file ide_project.c
 * @brief Implementation of desktop project handling utilities.
 */

#include "ide_project.h"

#include "utils/q16.h"
#include "utils/vec3.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif

static bool parse_float_token(const char *token, q16_16 *out)
{
    char *end = NULL;
    float value = strtof(token, &end);
    if ((end == token) || (out == NULL))
    {
        return false;
    }
    *out = q16_from_float(value);
    return true;
}

static spl_type_t parse_type(const char *token)
{
    if (strcmp(token, "BEZIER3") == 0)
    {
        return SPL_BEZIER3;
    }
    if (strcmp(token, "BSPLINE3") == 0)
    {
        return SPL_BSPLINE3;
    }
    if (strcmp(token, "QUINTIC") == 0)
    {
        return SPL_QUINTIC;
    }
    if (strcmp(token, "NURBS3") == 0)
    {
        return SPL_NURBS3;
    }
    return SPL_BEZIER3;
}

static void sanitise_name(const char *name, char *buffer, size_t capacity)
{
    size_t idx = 0U;
    for (; name[idx] != '\0' && idx + 1U < capacity; ++idx)
    {
        char c = name[idx];
        if (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')) ||
            ((c >= '0') && (c <= '9')))
        {
            buffer[idx] = c;
        }
        else
        {
            buffer[idx] = '_';
        }
    }
    buffer[idx] = '\0';
}

static bool ensure_directory(const char *path)
{
#ifdef _WIN32
    int rc = _mkdir(path);
    if ((rc != 0) && (errno != EEXIST))
    {
        return false;
    }
#else
    int rc = mkdir(path, 0777);
    if ((rc != 0) && (errno != EEXIST))
    {
        return false;
    }
#endif
    return true;
}

bool ide_project_load(const char *path, ide_project *project)
{
    if ((path == NULL) || (project == NULL))
    {
        return false;
    }

    FILE *fp = fopen(path, "r");
    if (fp == NULL)
    {
        return false;
    }

    memset(project, 0, sizeof(*project));
    project->requested_type = SPL_BEZIER3;
    project->period_ms = q16_from_int(1);
    project->limit_velocity = q16_from_int(200);
    project->limit_acceleration = q16_from_int(1000);
    project->limit_jerk = q16_from_int(5000);
    strncpy(project->name, "Unnamed", sizeof(project->name) - 1U);

    char line[256];
    while (fgets(line, sizeof(line), fp) != NULL)
    {
        if ((line[0] == '#') || (line[0] == '\n') || (line[0] == '\r'))
        {
            continue;
        }
        char key[32];
        if (sscanf(line, "%31s", key) != 1)
        {
            continue;
        }
        if (strcmp(key, "NAME") == 0)
        {
            const char *value = strchr(line, ' ');
            if (value != NULL)
            {
                value++;
                size_t len = strcspn(value, "\r\n");
                if (len >= sizeof(project->name))
                {
                    len = sizeof(project->name) - 1U;
                }
                strncpy(project->name, value, len);
                project->name[len] = '\0';
            }
        }
        else if (strcmp(key, "TYPE") == 0)
        {
            char type_token[32];
            if (sscanf(line, "%*s %31s", type_token) == 1)
            {
                project->requested_type = parse_type(type_token);
            }
        }
        else if (strcmp(key, "PERIOD_MS") == 0)
        {
            char period_token[32];
            if (sscanf(line, "%*s %31s", period_token) == 1)
            {
                (void)parse_float_token(period_token, &project->period_ms);
            }
        }
        else if (strcmp(key, "LIMITS") == 0)
        {
            char vel[32];
            char acc[32];
            char jerk[32];
            if (sscanf(line, "%*s %31s %31s %31s", vel, acc, jerk) == 3)
            {
                (void)parse_float_token(vel, &project->limit_velocity);
                (void)parse_float_token(acc, &project->limit_acceleration);
                (void)parse_float_token(jerk, &project->limit_jerk);
            }
        }
        else if (strcmp(key, "WAYPOINT") == 0)
        {
            if (project->waypoint_count >= IDE_MAX_WAYPOINTS)
            {
                continue;
            }
            char xs[32];
            char ys[32];
            char zs[32];
            if (sscanf(line, "%*s %31s %31s %31s", xs, ys, zs) == 3)
            {
                q16_16 x;
                q16_16 y;
                q16_16 z;
                if (parse_float_token(xs, &x) && parse_float_token(ys, &y) && parse_float_token(zs, &z))
                {
                    project->waypoints[project->waypoint_count].x = x;
                    project->waypoints[project->waypoint_count].y = y;
                    project->waypoints[project->waypoint_count].z = z;
                    project->waypoint_count++;
                }
            }
        }
    }

    fclose(fp);
    project->plan_ready = false;
    return (project->waypoint_count >= 2U);
}

static tinyml_segment_feature build_feature(const spl_plan_t *plan, const ide_project *project)
{
    (void)project;
    tinyml_segment_feature feat;
    feat.arc_length = plan->length;
    feat.target_velocity = project->limit_velocity;
    feat.target_accel = project->limit_acceleration;
    vec3_q16 p0;
    vec3_q16 p1;
    vec3_q16 p2;
    (void)spl_sample_arc(plan, q16_from_float(0.2f), &p0, NULL);
    (void)spl_sample_arc(plan, q16_from_float(0.5f), &p1, NULL);
    (void)spl_sample_arc(plan, q16_from_float(0.8f), &p2, NULL);
    vec3_q16 ab = vec3_sub(p1, p0);
    vec3_q16 bc = vec3_sub(p2, p1);
    vec3_q16 cross = vec3_cross(ab, bc);
    q16_16 numerator = vec3_norm(cross);
    q16_16 denominator = q16_mul(vec3_norm(ab), vec3_norm(bc));
    bool ok = true;
    if ((denominator == 0) || (numerator == 0))
    {
        feat.curvature = q16_from_int(0);
    }
    else
    {
        feat.curvature = q16_div(numerator, denominator, &ok);
        if (!ok)
        {
            feat.curvature = q16_from_int(0);
        }
    }
    return feat;
}

bool ide_project_generate(ide_project *project)
{
    if (project == NULL)
    {
        return false;
    }
    if (project->waypoint_count < 2U)
    {
        return false;
    }

    if (!spl_make_from_waypoints(project->waypoints,
                                 project->waypoint_count,
                                 NULL,
                                 NULL,
                                 &project->plan))
    {
        return false;
    }

    if (project->requested_type == SPL_NURBS3)
    {
        project->plan.type = SPL_NURBS3;
    }
    project->plan.max_vel = project->limit_velocity;
    project->plan.max_acc = project->limit_acceleration;
    project->plan.max_jerk = project->limit_jerk;

    tinyml_network_init(&project->optimiser);
    tinyml_segment_feature feat = build_feature(&project->plan, project);
    q16_16 scale = q16_from_int(1);
    if (tinyml_optimize_segments(&project->optimiser, &feat, 1U, &scale))
    {
        project->plan.max_vel = q16_mul(project->plan.max_vel, scale);
        project->plan.max_acc = q16_mul(project->plan.max_acc, scale);
        project->plan.max_jerk = q16_mul(project->plan.max_jerk, scale);
    }

    if (!spl_time_scale(&project->plan, project->period_ms, &project->trajectory))
    {
        project->plan_ready = false;
        return false;
    }

    project->plan_ready = true;
    return true;
}

static void write_report(const ide_project *project, FILE *fp)
{
    fprintf(fp, "Project: %s\n", project->name);
    fprintf(fp, "Samples: %zu\n", project->trajectory.count);
    float total_ms = q16_to_float(project->trajectory.period_ms) * (float)project->trajectory.count;
    fprintf(fp, "Total duration: %.3f s\n", total_ms / 1000.0f);
    fprintf(fp, "Velocity limit: %.2f\n", q16_to_float(project->plan.max_vel));
    fprintf(fp, "Acceleration limit: %.2f\n", q16_to_float(project->plan.max_acc));
    fprintf(fp, "Jerk limit: %.2f\n", q16_to_float(project->plan.max_jerk));
}

bool ide_project_export(const ide_project *project, const char *directory)
{
    if ((project == NULL) || (directory == NULL) || (!project->plan_ready))
    {
        return false;
    }

    if (!ensure_directory(directory))
    {
        return false;
    }

    char safe_name[64];
    sanitise_name(project->name, safe_name, sizeof(safe_name));

    configurator_profile profile;
    configurator_default_profile(&profile);

    char path_buffer[256];
    snprintf(path_buffer, sizeof(path_buffer), "%s/%s_ui.ppm", directory, safe_name);
    if (!configurator_render_ui(&profile, path_buffer))
    {
        return false;
    }

    snprintf(path_buffer, sizeof(path_buffer), "%s/%s_ui.json", directory, safe_name);
    if (!configurator_export_profile(&profile, path_buffer))
    {
        return false;
    }

    snprintf(path_buffer, sizeof(path_buffer), "%s/%s_traj.obj", directory, safe_name);
    if (!visualizer3d_export_obj(&project->trajectory, path_buffer))
    {
        return false;
    }

    visualizer3d_camera cam;
    cam.position = vec3_from_float(200.0f, -160.0f, 220.0f);
    if (project->trajectory.count > 0U)
    {
        cam.target = project->trajectory.position[0];
    }
    else
    {
        cam.target = project->waypoints[0];
    }
    cam.fov_deg = q16_from_float(55.0f);

    snprintf(path_buffer, sizeof(path_buffer), "%s/%s_traj.ppm", directory, safe_name);
    if (!visualizer3d_render_ppm(&project->trajectory, &cam, path_buffer))
    {
        return false;
    }

    snprintf(path_buffer, sizeof(path_buffer), "%s/%s_report.txt", directory, safe_name);
    FILE *report = fopen(path_buffer, "w");
    if (report == NULL)
    {
        return false;
    }
    write_report(project, report);
    fclose(report);

    return true;
}
