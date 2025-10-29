/**
 * @file delta_kinematics.h
 * @brief Analytical kinematics for delta parallel robot.
 */

#ifndef DELTA_KINEMATICS_H
#define DELTA_KINEMATICS_H

#include <stdbool.h>

#include "utils/vec3.h"
#include "utils/matrix3.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Delta robot configuration constants. */
typedef struct
{
    q16_16 R_base;              /**< Radius of base triangle. */
    q16_16 r_eff;               /**< Radius of end-effector triangle. */
    q16_16 L_upper;             /**< Length of upper arms. */
    q16_16 L_lower;             /**< Length of parallelogram legs. */
    q16_16 z_offset;            /**< Vertical offset between base and neutral plane. */
    q16_16 soft_xyz_min[3];     /**< Soft minimum workspace limits. */
    q16_16 soft_xyz_max[3];     /**< Soft maximum workspace limits. */
    q16_16 calib_offsets[3];    /**< Encoder offsets. */
    q16_16 scale_per_axis[3];   /**< Scale corrections per axis. */
} delta_cfg_t;

/**
 * @brief Populate configuration with conservative defaults.
 * @param cfg Configuration instance to populate.
 */
void delta_default_config(delta_cfg_t *cfg);

/**
 * @brief Compute forward kinematics from joint angles.
 * @param cfg Robot configuration.
 * @param theta Joint angles in radians (Q16.16).
 * @param position Output end-effector position.
 * @return True when solution exists.
 */
bool delta_forward_kinematics(const delta_cfg_t *cfg, const q16_16 theta[3], vec3_q16 *position);

/**
 * @brief Compute inverse kinematics from Cartesian target.
 * @param cfg Robot configuration.
 * @param position Target Cartesian point.
 * @param theta Output joint angles in radians (Q16.16).
 * @return True when target is reachable.
 */
bool delta_inverse_kinematics(const delta_cfg_t *cfg, const vec3_q16 *position, q16_16 theta[3]);

/**
 * @brief Compute Jacobian matrix mapping joint velocities to Cartesian velocity.
 * @param cfg Robot configuration.
 * @param theta Joint angles.
 * @param jacobian Output Jacobian matrix.
 * @return True when Jacobian is well-conditioned.
 */
bool delta_jacobian(const delta_cfg_t *cfg, const q16_16 theta[3], matrix3_q16 *jacobian);

/**
 * @brief Check workspace limits.
 * @param cfg Robot configuration.
 * @param position Cartesian point.
 * @return True if within soft limits.
 */
bool delta_within_workspace(const delta_cfg_t *cfg, const vec3_q16 *position);

#ifdef __cplusplus
}
#endif

#endif
