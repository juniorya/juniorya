/**
 * @file simulator.c
 * @brief Desktop simulator for delta CNC trajectories with 3D visualisation.
 */

#include "configurator.h"
#include "renderer.h"
#include "visualizer3d.h"

#include "board/board.h"
#include "planner/splines/splines.h"
#include "utils/vec3.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void build_demo_trajectory(time_scaled_traj_t *traj)
{
    vec3_q16 pts[4];
    pts[0] = vec3_from_float(-50.0f, 0.0f, -50.0f);
    pts[1] = vec3_from_float(0.0f, 50.0f, -40.0f);
    pts[2] = vec3_from_float(50.0f, 0.0f, -30.0f);
    pts[3] = vec3_from_float(0.0f, -50.0f, -20.0f);
    spl_plan_t plan;
    (void)spl_make_from_waypoints(pts, 4U, NULL, NULL, &plan);
    (void)spl_time_scale(&plan, q16_from_int(1), traj);
}

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    time_scaled_traj_t traj;
    build_demo_trajectory(&traj);

    renderer_canvas canvas;
    if (!renderer_init(&canvas, 512U, 512U))
    {
        fprintf(stderr, "Failed to initialise renderer\n");
        return 1;
    }
    renderer_clear(&canvas);
    for (size_t i = 0; i < traj.count; ++i)
    {
        renderer_plot(&canvas,
                      q16_to_float(traj.position[i].x),
                      q16_to_float(traj.position[i].y));
    }
    renderer_save_ppm(&canvas, "sim_output.ppm");
    renderer_save_csv(&traj, "sim_output.csv");
    renderer_deinit(&canvas);

    visualizer3d_camera cam;
    cam.position = vec3_from_float(200.0f, -200.0f, 300.0f);
    cam.target = vec3_from_float(0.0f, 0.0f, -40.0f);
    cam.fov_deg = q16_from_float(60.0f);
    (void)visualizer3d_export_obj(&traj, "sim_output.obj");
    (void)visualizer3d_render_ppm(&traj, &cam, "sim_output_3d.ppm");

    configurator_profile profile;
    configurator_default_profile(&profile);
    (void)configurator_render_ui(&profile, "sim_configurator.ppm");
    (void)configurator_export_profile(&profile, "sim_profile.json");

    printf("Simulator output written to sim_output.ppm, sim_output_3d.ppm, sim_output.obj and sim_profile.json\n");
    return 0;
}
