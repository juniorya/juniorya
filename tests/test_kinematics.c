#include "test_suite.h"
#include "../kinematics/delta.h"
#include <assert.h>

void test_kinematics(void)
{
    delta_cfg_t cfg;
    delta_cfg_defaults(&cfg);
    delta_joints_t joints = {
        .theta = {fixed_from_double(0.0), fixed_from_double(0.0), fixed_from_double(0.0)}
    };
    delta_pose_t pose;
    assert(delta_forward_kinematics(&cfg, &joints, &pose));
    delta_joints_t solved;
    assert(delta_inverse_kinematics(&cfg, &pose, &solved));
    delta_pose_t pose_back;
    assert(delta_forward_kinematics(&cfg, &solved, &pose_back));
}
