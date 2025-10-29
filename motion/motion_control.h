#ifndef MOTION_MOTION_CONTROL_H
#define MOTION_MOTION_CONTROL_H

#include "planner/planner.h"
#include "kinematics/delta.h"
#include "cia402/cia402.h"
#include "ethcat/master.h"

typedef struct {
    planner_queue_t *planner;
    ethcat_master_t *master;
    cia402_axis_t *axes;
    delta_pose_t command_pose;
    delta_joint_t joint_command;
    delta_joint_t joint_previous;
    q16_16_t feedforward_torque[3];
} motion_controller_t;

void motion_controller_init(motion_controller_t *motion, planner_queue_t *planner, ethcat_master_t *master, cia402_axis_t *axes);
void motion_controller_tick(motion_controller_t *motion);

#endif
