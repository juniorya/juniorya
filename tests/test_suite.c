/**
 * @file test_suite.c
 * @brief Minimal unit test coverage for planner, kinematics and storage.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "board/board.h"
#include "kinematics/delta_kinematics.h"
#include "ml/tinyml_optimizer.h"
#include "planner/splines/splines.h"
#include "sim/configurator.h"
#include "sim/visualizer3d.h"
#include "storage/storage.h"
#include "utils/vec3.h"

static void test_spline_generation(void)
{
    vec3_q16 pts[4];
    for (int i = 0; i < 4; ++i)
    {
        pts[i] = vec3_from_float((float)i * 10.0f, 0.0f, 0.0f);
    }
    spl_plan_t plan;
    assert(spl_make_from_waypoints(pts, 4U, NULL, NULL, &plan));
    vec3_q16 pos;
    vec3_q16 deriv;
    assert(spl_sample_arc(&plan, q16_from_float(0.5f), &pos, &deriv));
}

static void test_kinematics_inverse(void)
{
    delta_cfg_t cfg;
    delta_default_config(&cfg);
    vec3_q16 pos = vec3_from_float(0.0f, 0.0f, q16_to_float(cfg.z_offset));
    q16_16 theta[3];
    assert(delta_inverse_kinematics(&cfg, &pos, theta));
}

static void test_tinyml_example(void)
{
    tinyml_network net;
    tinyml_network_init(&net);
    tinyml_segment_feature feat;
    feat.curvature = q16_from_float(0.05f);
    feat.arc_length = q16_from_float(120.0f);
    feat.target_velocity = q16_from_float(150.0f);
    feat.target_accel = q16_from_float(800.0f);
    q16_16 scale;
    assert(tinyml_optimize_segments(&net, &feat, 1U, &scale));
}

static void test_storage_roundtrip(void)
{
    storage_init_options opts;
    storage_default_options(&opts);
    assert(storage_init(&opts));
    char payload[] = "test";
    assert(storage_write(payload, sizeof(payload)));
    char buf[8];
    assert(storage_read(buf, sizeof(buf)));
    assert(strcmp(buf, payload) == 0);
    storage_shutdown();
}

static void test_visual_outputs(void)
{
    configurator_profile profile;
    configurator_default_profile(&profile);
    assert(configurator_render_ui(&profile, "test_ui.ppm"));
    assert(configurator_export_profile(&profile, "test_profile.json"));
    vec3_q16 pts[4];
    pts[0] = vec3_from_float(0.0f, 0.0f, 0.0f);
    pts[1] = vec3_from_float(50.0f, 50.0f, -20.0f);
    pts[2] = vec3_from_float(100.0f, 0.0f, -40.0f);
    pts[3] = vec3_from_float(150.0f, -30.0f, -50.0f);
    spl_plan_t plan;
    assert(spl_make_from_waypoints(pts, 4U, NULL, NULL, &plan));
    time_scaled_traj_t traj;
    assert(spl_time_scale(&plan, q16_from_int(1), &traj));
    visualizer3d_camera cam;
    cam.position = vec3_from_float(150.0f, -120.0f, 180.0f);
    cam.target = vec3_from_float(50.0f, 25.0f, -20.0f);
    cam.fov_deg = q16_from_float(55.0f);
    assert(visualizer3d_export_obj(&traj, "test_traj.obj"));
    assert(visualizer3d_render_ppm(&traj, &cam, "test_traj.ppm"));
    (void)remove("test_ui.ppm");
    (void)remove("test_profile.json");
    (void)remove("test_traj.obj");
    (void)remove("test_traj.ppm");
}

int main(void)
{
    test_spline_generation();
    test_kinematics_inverse();
    test_tinyml_example();
    test_storage_roundtrip();
    test_visual_outputs();
    printf("All tests passed\n");
    return 0;
}
