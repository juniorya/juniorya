#ifndef KINEMATICS_DELTA_H
#define KINEMATICS_DELTA_H

#include "utils/fixed.h"
#include "utils/matrix.h"

typedef struct {
    q16_16_t R_base;
    q16_16_t r_eff;
    q16_16_t L_upper;
    q16_16_t L_lower;
    q16_16_t z_offset;
    q16_16_t soft_xyz_min[3];
    q16_16_t soft_xyz_max[3];
} delta_cfg_t;

typedef struct {
    q16_16_t theta[3];
} delta_joint_t;

typedef struct {
    q16_16_t xyz[3];
} delta_pose_t;

typedef struct {
    mat3x3_q16_16_t jacobian;
    q16_16_t det;
    bool singular;
} delta_jacobian_t;

void delta_init(const delta_cfg_t *cfg);
bool delta_inverse_kinematics(const delta_pose_t *cart, delta_joint_t *joints);
bool delta_forward_kinematics(const delta_joint_t *joints, delta_pose_t *cart);
void delta_compute_jacobian(const delta_joint_t *joints, delta_jacobian_t *out);
bool delta_within_workspace(const delta_pose_t *cart);

#endif
