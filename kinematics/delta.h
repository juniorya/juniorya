#ifndef KINEMATICS_DELTA_H
#define KINEMATICS_DELTA_H

#include <stdbool.h>

#include "utils/vec3.h"

/** \brief Delta robot configuration. */
typedef struct
{
    q16_16 R_base;             /**< Base radius. */
    q16_16 r_eff;              /**< End effector radius. */
    q16_16 L_upper;            /**< Upper arm length. */
    q16_16 L_lower;            /**< Lower arm length. */
    q16_16 z_offset;           /**< Mechanical zero offset. */
    q16_16 soft_xyz_min[3];    /**< Soft minimum workspace. */
    q16_16 soft_xyz_max[3];    /**< Soft maximum workspace. */
    q16_16 calib_offsets[3];   /**< Encoder offsets. */
    q16_16 scale_per_axis[3];  /**< Axis scaling factors. */
} delta_cfg_t;

/** \brief Initialise configuration with defaults. */
void delta_default_config(delta_cfg_t *cfg);

/** \brief Inverse kinematics for the delta robot. */
bool delta_inverse(const delta_cfg_t *cfg, vec3_q16 xyz, q16_16 joints[3]);

/** \brief Forward kinematics for the delta robot. */
bool delta_forward(const delta_cfg_t *cfg, const q16_16 joints[3], vec3_q16 *xyz);

/** \brief Jacobian determinant heuristic for singularity detection. */
q16_16 delta_jacobian_det(const delta_cfg_t *cfg, const q16_16 joints[3]);

/** \brief Validate workspace boundaries. */
bool delta_is_within_workspace(const delta_cfg_t *cfg, vec3_q16 xyz);

#endif
