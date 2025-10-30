/**
 * @file test_suite.c
 * @brief Minimal unit test coverage for planner, kinematics and storage.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif

#include "board/board.h"
#include "kinematics/delta_kinematics.h"
#include "ml/tinyml_optimizer.h"
#include "ml/tinyml_configurator.h"
#include "cia402/cia402.h"
#include "cia402/fb.h"
#include "ethcat/xml_loader.h"
#include "ide/ide_project.h"
#include "ide/st_compiler.h"
#include "gcode/gcode.h"
#include "planner/planner.h"
#include "planner/splines/splines.h"
#include "sim/configurator.h"
#include "sim/visualizer3d.h"
#include "storage/storage.h"
#include "vision/vision.h"
#include "utils/vec3.h"

#ifndef TEST_DATA_DIR
#define TEST_DATA_DIR "."
#endif

#define TEST_DATA_PATH(name) TEST_DATA_DIR "/" name

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

static void test_tinyml_profile(void)
{
    tinyml_profile profile;
    tinyml_profile_default(&profile);
    assert(tinyml_profile_load(TEST_DATA_PATH("tinyml_profile.ini"), &profile));
    tinyml_network net;
    tinyml_network_init(&net);
    assert(tinyml_profile_apply(&profile, &net));
}

static void test_cia402_function_blocks(void)
{
    cia402_axis axis;
    cia402_axis_init(&axis, 0U);
    cia402_mc_power power;
    cia402_mc_power_init(&power);
    power.enable = true;
    for (int i = 0; i < 8; ++i)
    {
        cia402_mc_power_cycle(&power, &axis);
        cia402_step(&axis);
    }
    assert(power.status);
    assert(axis.state == CIA402_STATE_OPERATION_ENABLED);

    cia402_mc_move_absolute move_abs;
    cia402_mc_move_absolute_init(&move_abs);
    move_abs.execute = true;
    move_abs.position = q16_from_float(12.5f);
    move_abs.velocity = q16_from_float(2.0f);
    for (int i = 0; i < 64 && !move_abs.done; ++i)
    {
        cia402_mc_move_absolute_cycle(&move_abs, &axis);
        cia402_step(&axis);
    }
    assert(move_abs.done);
    assert(q16_abs(q16_sub(axis.actual_position, q16_from_float(12.5f))) < q16_from_float(0.1f));

    cia402_mc_move_relative move_rel;
    cia402_mc_move_relative_init(&move_rel);
    move_rel.execute = true;
    move_rel.distance = q16_from_float(-2.5f);
    move_rel.velocity = q16_from_float(1.0f);
    for (int i = 0; i < 64 && !move_rel.done; ++i)
    {
        cia402_mc_move_relative_cycle(&move_rel, &axis);
        cia402_step(&axis);
    }
    assert(move_rel.done);

    cia402_mc_move_velocity move_vel;
    cia402_mc_move_velocity_init(&move_vel);
    move_vel.execute = true;
    move_vel.velocity = q16_from_float(0.5f);
    for (int i = 0; i < 32; ++i)
    {
        cia402_mc_move_velocity_cycle(&move_vel, &axis);
        cia402_step(&axis);
    }
    move_vel.execute = false;
    cia402_mc_move_velocity_cycle(&move_vel, &axis);
    cia402_step(&axis);
    assert(move_vel.done);

    cia402_mc_torque_control torque;
    cia402_mc_torque_control_init(&torque);
    torque.execute = true;
    torque.torque = q16_from_float(0.2f);
    torque.velocity = q16_from_float(0.1f);
    for (int i = 0; i < 32 && !torque.done; ++i)
    {
        cia402_mc_torque_control_cycle(&torque, &axis);
        cia402_step(&axis);
    }
    assert(torque.done);

    cia402_mc_stop stop;
    cia402_mc_stop_init(&stop);
    stop.execute = true;
    for (int i = 0; i < 64 && !stop.done; ++i)
    {
        cia402_mc_stop_cycle(&stop, &axis);
        cia402_step(&axis);
    }
    assert(stop.done);
    stop.execute = false;
    cia402_mc_stop_cycle(&stop, &axis);
    cia402_step(&axis);

    cia402_mc_reset reset;
    cia402_mc_reset_init(&reset);
    axis.fault_active = true;
    axis.state = CIA402_STATE_FAULT;
    reset.execute = true;
    for (int i = 0; i < 4; ++i)
    {
        cia402_mc_reset_cycle(&reset, &axis);
        cia402_step(&axis);
    }
    assert(reset.done);
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

static void test_gcode_parser(void)
{
    planner_config cfg;
    planner_default_config(&cfg);
    cfg.queue_length = 8U;
    planner_context planner;
    assert(planner_init(&planner, &cfg));

    gcode_parser parser;
    gcode_parser_init(&parser, true);
    assert(gcode_parse_line(&parser, &planner, "G90"));
    assert(gcode_parse_line(&parser, &planner, "F1500"));
    assert(gcode_parse_line(&parser, &planner, "G0 X10.0 Y5.0 Z-2.0 F1200"));
    assert(gcode_parse_line(&parser, &planner, "G2 X5.0 Y10.0 I0 J5.0"));
    const vec3_q16 *pos = gcode_parser_current_position(&parser);
    assert(pos != NULL);
    const char *report = gcode_parser_last_report(&parser);
    assert(report != NULL);
    planner_shutdown(&planner);
}

static void test_st_compilation(void)
{
    stc_compilation comp;
    assert(stc_compile_file(TEST_DATA_PATH("demo_program.st"), &comp));
    assert(comp.c_length > 0U);
    assert(comp.token_count > 0U);
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

static void test_ethcat_xml_loader(void)
{
    ethcat_master_config cfg;
    ethcat_master_default_config(&cfg);
    assert(ethcat_load_drives_from_xml(TEST_DATA_PATH("keba_drive.xml"), &cfg));
    assert(cfg.drive_count >= 1U);
    assert(cfg.drives[0].vendor_id != 0U);
}

static void test_vision_alignment(void)
{
    vision_config cfg;
    vision_default_config(&cfg);
    vision_context ctx;
    assert(vision_init(&ctx, &cfg));
    assert(vision_process_frame(&ctx));
    q16_16 error = 0;
    assert(vision_get_alignment_error(&ctx, &error));
    vision_shutdown(&ctx);
}

static void test_ide_tooling(void)
{
    ide_project project;
    assert(ide_project_load(TEST_DATA_PATH("demo_project.cncp"), &project));
    strncpy(project.optimiser_profile_path,
            TEST_DATA_PATH("tinyml_profile.ini"),
            sizeof(project.optimiser_profile_path) - 1U);
    project.optimiser_profile_path[sizeof(project.optimiser_profile_path) - 1U] = '\0';
    strncpy(project.drive_xml_path,
            TEST_DATA_PATH("keba_drive.xml"),
            sizeof(project.drive_xml_path) - 1U);
    project.drive_xml_path[sizeof(project.drive_xml_path) - 1U] = '\0';
    strncpy(project.st_program_path,
            TEST_DATA_PATH("demo_program.st"),
            sizeof(project.st_program_path) - 1U);
    project.st_program_path[sizeof(project.st_program_path) - 1U] = '\0';
    assert(ide_project_generate(&project));
    assert(project.plan_ready);
    assert(project.drive_xml_loaded);
    const char *out_dir = "ide_test_output";
    assert(ide_project_export(&project, out_dir));
    assert(access("ide_test_output/Demo_Workspace_st.c", F_OK) == 0);
    assert(access("ide_test_output/Demo_Workspace_st_metadata.json", F_OK) == 0);
    assert(access("ide_test_output/Demo_Workspace_debug.csv", F_OK) == 0);
    (void)remove("ide_test_output/Demo_Workspace_ui.ppm");
    (void)remove("ide_test_output/Demo_Workspace_ui.json");
    (void)remove("ide_test_output/Demo_Workspace_traj.obj");
    (void)remove("ide_test_output/Demo_Workspace_traj.ppm");
    (void)remove("ide_test_output/Demo_Workspace_report.txt");
    (void)remove("ide_test_output/Demo_Workspace_st.c");
    (void)remove("ide_test_output/Demo_Workspace_st_metadata.json");
    (void)remove("ide_test_output/Demo_Workspace_debug.csv");
#ifdef _WIN32
    (void)_rmdir(out_dir);
#else
    (void)rmdir(out_dir);
#endif
}

int main(void)
{
    test_spline_generation();
    test_kinematics_inverse();
    test_tinyml_example();
    test_tinyml_profile();
    test_cia402_function_blocks();
    test_storage_roundtrip();
    test_gcode_parser();
    test_st_compilation();
    test_visual_outputs();
    test_ide_tooling();
    test_ethcat_xml_loader();
    test_vision_alignment();
    printf("All tests passed\n");
    return 0;
}
