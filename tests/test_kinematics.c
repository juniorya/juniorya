#include "test_suite.h"

#include <assert.h>

#include "../kinematics/delta_kinematics.h"

void test_kinematics(void)
{
    delta_cfg_t cfg = {
        .R_base = q16_from_int(200),
        .r_eff = q16_from_int(50),
        .L_upper = q16_from_int(250),
        .L_lower = q16_from_int(500),
        .z_offset = q16_from_int(100),
        .soft_xyz_min = {q16_from_int(-200), q16_from_int(-200), q16_from_int(-600)},
        .soft_xyz_max = {q16_from_int(200), q16_from_int(200), q16_from_int(-100)},
        .calib_offsets = {0, 0, 0},
        .scale_per_axis = {Q16_16_ONE, Q16_16_ONE, Q16_16_ONE}};

    vec3_q16 target = {q16_from_int(0), q16_from_int(0), q16_from_int(-300)};
    q16_16 joints[3];
    assert(delta_inverse_kinematics(&cfg, &target, joints) == DELTA_KIN_OK);
}
