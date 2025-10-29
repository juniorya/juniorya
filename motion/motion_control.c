#include "motion_control.h"
#include <stddef.h>
#include "utils/fixed.h"

void motion_controller_init(motion_controller_t *motion, planner_queue_t *planner, ethcat_master_t *master, cia402_axis_t *axes)
{
    motion->planner = planner;
    motion->master = master;
    motion->axes = axes;
    for (int i = 0; i < 3; ++i) {
        motion->command_pose.xyz[i] = 0;
        motion->joint_command.theta[i] = 0;
        motion->joint_previous.theta[i] = 0;
        motion->feedforward_torque[i] = 0;
    }
}

static void build_targets(const motion_controller_t *motion, int axis, q16_16_t *targets)
{
    targets[0] = motion->joint_command.theta[axis];
    q16_16_t delta = motion->joint_command.theta[axis] - motion->joint_previous.theta[axis];
    targets[1] = delta; /* per control period */
    targets[2] = motion->feedforward_torque[axis];
}

void motion_controller_tick(motion_controller_t *motion)
{
    delta_pose_t pose;
    if (!planner_step(motion->planner, &pose)) {
        pose = motion->command_pose;
    }

    delta_joint_t joints;
    if (!delta_inverse_kinematics(&pose, &joints)) {
        for (int axis = 0; axis < 3; ++axis) {
            motion->axes[axis].quick_stop = true;
        }
        return;
    }

    for (int axis = 0; axis < 3; ++axis) {
        const ethcat_txpdo_t *feedback = ethcat_master_get_feedback(motion->master, axis);
        if (feedback != NULL) {
            cia402_axis_update(&motion->axes[axis], feedback);
        }
    }

    for (int axis = 0; axis < 3; ++axis) {
        motion->joint_previous.theta[axis] = motion->joint_command.theta[axis];
        motion->joint_command.theta[axis] = joints.theta[axis];
        q16_16_t targets[3];
        build_targets(motion, axis, targets);
        cia402_axis_command(&motion->axes[axis], targets, motion->axes[axis].mode);
        ethcat_rxpdo_t rx;
        cia402_axis_build_rxpdo(&motion->axes[axis], &rx);
        ethcat_master_set_target(motion->master, axis, &rx);
    }
    motion->command_pose = pose;
}
