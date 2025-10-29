#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "kinematics/delta.h"
#include "planner/planner.h"
#include "board/config.h"

static void test_delta_kinematics(void)
{
    delta_cfg_t cfg = {
        .R_base = q16_16_from_float(0.300f),
        .r_eff = q16_16_from_float(0.100f),
        .L_upper = q16_16_from_float(0.300f),
        .L_lower = q16_16_from_float(0.400f),
        .z_offset = q16_16_from_float(0.200f),
        .soft_xyz_min = {q16_16_from_float(-0.200f), q16_16_from_float(-0.200f), q16_16_from_float(-0.500f)},
        .soft_xyz_max = {q16_16_from_float(0.200f), q16_16_from_float(0.200f), q16_16_from_float(-0.100f)}
    };
    delta_init(&cfg);
    delta_pose_t pose = {
        .xyz = {q16_16_from_float(0.0f), q16_16_from_float(0.0f), q16_16_from_float(-0.400f)}
    };
    delta_joint_t joints;
    if (!delta_inverse_kinematics(&pose, &joints)) {
        fprintf(stderr, "IK failed\n");
        exit(1);
    }
    delta_pose_t reconstructed;
    if (!delta_forward_kinematics(&joints, &reconstructed)) {
        fprintf(stderr, "FK failed\n");
        exit(1);
    }
    for (int axis = 0; axis < 3; ++axis) {
        float expected = q16_16_to_float(pose.xyz[axis]);
        float actual = q16_16_to_float(reconstructed.xyz[axis]);
        if (fabsf(expected - actual) > 0.005f) {
            fprintf(stderr, "Delta mismatch axis %d: expected %.4f actual %.4f\n", axis, expected, actual);
            exit(1);
        }
    }
}

static void test_planner_progress(void)
{
    planner_queue_t planner;
    planner_init(&planner, 1000U);
    delta_pose_t target = {
        .xyz = {q16_16_from_float(0.050f), q16_16_from_float(0.0f), q16_16_from_float(-0.320f)}
    };
    if (!planner_push_line(&planner, &target, q16_16_from_float(0.100f), q16_16_from_float(1.0f), q16_16_from_float(5.0f))) {
        fprintf(stderr, "Planner push failed\n");
        exit(1);
    }
    delta_pose_t pose;
    q16_16_t last_x = 0;
    while (planner_step(&planner, &pose)) {
        if (pose.xyz[0] < last_x) {
            fprintf(stderr, "Planner monotonicity failure\n");
            exit(1);
        }
        last_x = pose.xyz[0];
    }
    if (q16_16_abs(pose.xyz[0] - target.xyz[0]) > q16_16_from_float(0.0005f)) {
        fprintf(stderr, "Planner final position mismatch\n");
        exit(1);
    }
}

int main(void)
{
    test_delta_kinematics();
    test_planner_progress();
    printf("All tests passed\n");
    return 0;
}
