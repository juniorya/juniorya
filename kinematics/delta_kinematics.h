/**
 * @file delta_kinematics.h
 * @brief Forward and inverse kinematics for a classical delta robot.
 *
 * The implementation follows the derivations from "Modelling and Control of a
 * Delta Robot" by Clavel et al. The inverse kinematics (IK) solves joint angles
 * for a given Cartesian position while the forward kinematics (FK) uses the
 * Newton-Raphson method with an analytic Jacobian. All public functions guard the
 * workspace boundaries defined in ::delta_cfg_t and fall back to safe error
 * codes when a request cannot be satisfied.
 */
#ifndef KINEMATICS_DELTA_KINEMATICS_H
#define KINEMATICS_DELTA_KINEMATICS_H

#include <stdbool.h>
#include <stddef.h>

#include "utils/q16.h"
#include "utils/vec3.h"

/**
 * @brief Configuration parameters of the delta robot geometry.
 */
typedef struct
{
    q16_16 R_base;           /**< Radius of the base platform [mm]. */
    q16_16 r_eff;            /**< Radius of the end-effector platform [mm]. */
    q16_16 L_upper;          /**< Length of the upper arms [mm]. */
    q16_16 L_lower;          /**< Length of the lower arms [mm]. */
    q16_16 z_offset;         /**< Vertical offset between motor plane and origin [mm]. */
    q16_16 soft_xyz_min[3];  /**< Minimum allowed Cartesian coordinates. */
    q16_16 soft_xyz_max[3];  /**< Maximum allowed Cartesian coordinates. */
    q16_16 calib_offsets[3]; /**< Encoder offsets for each axis. */
    q16_16 scale_per_axis[3];/**< Encoder scale factors. */
} delta_cfg_t;

/**
 * @brief Result codes reported by kinematics operations.
 */
typedef enum
{
    DELTA_KIN_OK = 0,          /**< Operation succeeded. */
    DELTA_KIN_OUT_OF_RANGE,    /**< Requested point is outside workspace. */
    DELTA_KIN_SINGULARITY      /**< Jacobian is singular or near-singular. */
} delta_kin_result_t;

/**
 * @brief Solve inverse kinematics for the given Cartesian point.
 *
 * The equations originate from the spherical intersection of each parallelogram
 * arm with the platform. The method solves the quadratic equation derived in
 * Clavel's paper and ensures that the joint limits implicit in the geometry are
 * respected.
 *
 * @param cfg Delta configuration.
 * @param xyz Cartesian target point.
 * @param joint_angles_q16 Output joint angles in radians (Q16.16).
 * @return Result code indicating success or failure.
 */
delta_kin_result_t delta_inverse_kinematics(const delta_cfg_t *cfg,
                                            const vec3_q16 *xyz,
                                            q16_16 joint_angles_q16[3]);

/**
 * @brief Solve forward kinematics given joint angles.
 *
 * The solver uses an iterative Newton-Raphson approach initialized with the
 * geometric center of the reachable workspace. Convergence is typically achieved
 * in four iterations for well-conditioned configurations.
 *
 * @param cfg Delta configuration.
 * @param joint_angles_q16 Joint angles in radians (Q16.16).
 * @param xyz Output Cartesian position.
 * @return Result code indicating success or failure.
 */
delta_kin_result_t delta_forward_kinematics(const delta_cfg_t *cfg,
                                            const q16_16 joint_angles_q16[3],
                                            vec3_q16 *xyz);

/**
 * @brief Compute Jacobian matrix for the supplied joint configuration.
 *
 * The Jacobian relates joint velocity to Cartesian velocity. It is computed via
 * analytical derivatives derived from the IK equations. Singularities are
 * detected when the determinant falls below a configurable threshold, in which
 * case ::DELTA_KIN_SINGULARITY is returned.
 *
 * @param cfg Delta configuration.
 * @param joint_angles_q16 Joint angles in radians (Q16.16).
 * @param jacobian_q16 Output 3x3 matrix in row-major order.
 * @return Result code indicating success or failure.
 */
delta_kin_result_t delta_jacobian(const delta_cfg_t *cfg,
                                  const q16_16 joint_angles_q16[3],
                                  q16_16 jacobian_q16[9]);

/**
 * @brief Verify that a Cartesian point lies within the configured workspace.
 *
 * @param cfg Delta configuration.
 * @param xyz Cartesian point.
 * @return true if the point is allowed, false otherwise.
 */
bool delta_point_in_workspace(const delta_cfg_t *cfg, const vec3_q16 *xyz);

#endif /* KINEMATICS_DELTA_KINEMATICS_H */
