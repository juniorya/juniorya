/**
 * @file delta_kinematics.c
 * @brief Delta robot kinematics implementation.
 *
 * The inverse kinematics derives from the standard delta geometry using the
 * intersection of spheres with radius ::delta_cfg_t::L_lower and centers defined
 * by the upper arm positions. The forward kinematics solves the nonlinear system
 * \f$f_i(x, y, z) = 0\f$ using Newton-Raphson with the analytic Jacobian.
 */
#include "delta_kinematics.h"

#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define Q16_PI q16_from_float(3.1415926f)

static void apply_calibration(const delta_cfg_t *cfg, q16_16 raw_angles[3])
{
    for (size_t i = 0U; i < 3U; ++i)
    {
        raw_angles[i] = q16_add(q16_mul(raw_angles[i], cfg->scale_per_axis[i]),
                                 cfg->calib_offsets[i]);
    }
}

delta_kin_result_t delta_inverse_kinematics(const delta_cfg_t *cfg,
                                            const vec3_q16 *xyz,
                                            q16_16 joint_angles_q16[3])
{
    if (!delta_point_in_workspace(cfg, xyz))
    {
        return DELTA_KIN_OUT_OF_RANGE;
    }

    const float fx = q16_to_float(xyz->x);
    const float fy = q16_to_float(xyz->y);
    const float R = q16_to_float(cfg->R_base);
    const float r = q16_to_float(cfg->r_eff);
    const float L = q16_to_float(cfg->L_upper);
    const float l = q16_to_float(cfg->L_lower);

    for (size_t i = 0U; i < 3U; ++i)
    {
        const float angle = (float)(i * 2U) * (float)M_PI / 3.0f;
        const float cx = R * cosf(angle);
        const float cy = R * sinf(angle);
        const float ex = r * cosf(angle + (float)M_PI / 3.0f);
        const float ey = r * sinf(angle + (float)M_PI / 3.0f);
        const float px = fx + ex - cx;
        const float py = fy + ey - cy;
        const float d = sqrtf(px * px + py * py);
        if (d > (L + l))
        {
            return DELTA_KIN_OUT_OF_RANGE;
        }
        const float theta = acosf((L * L + d * d - l * l) / (2.0f * L * d));
        float result = atan2f(py, px) - theta;
        if (result > (float)M_PI)
        {
            result -= 2.0f * (float)M_PI;
        }
        if (result < -(float)M_PI)
        {
            result += 2.0f * (float)M_PI;
        }
        joint_angles_q16[i] = q16_from_float(result);
    }

    apply_calibration(cfg, joint_angles_q16);
    return DELTA_KIN_OK;
}

static void forward_equations(const delta_cfg_t *cfg,
                              const q16_16 joint_angles_q16[3],
                              vec3_q16 *xyz,
                              q16_16 residuals[3])
{
    const float R = q16_to_float(cfg->R_base);
    const float r = q16_to_float(cfg->r_eff);
    const float L = q16_to_float(cfg->L_upper);
    const float l = q16_to_float(cfg->L_lower);
    const float x = q16_to_float(xyz->x);
    const float y = q16_to_float(xyz->y);
    const float z = q16_to_float(xyz->z) + q16_to_float(cfg->z_offset);

    for (size_t i = 0U; i < 3U; ++i)
    {
        const float angle = (float)(i * 2U) * (float)M_PI / 3.0f;
        const float cx = R * cosf(angle);
        const float cy = R * sinf(angle);
        const float ex = r * cosf(angle + (float)M_PI / 3.0f);
        const float ey = r * sinf(angle + (float)M_PI / 3.0f);
        const float theta = q16_to_float(joint_angles_q16[i]);
        const float ux = cx + L * cosf(theta);
        const float uy = cy + L * sinf(theta);
        const float uz = q16_to_float(cfg->z_offset);
        const float dx = x + ex - ux;
        const float dy = y + ey - uy;
        const float dz = z - uz;
        const float err = (dx * dx + dy * dy + dz * dz) - (l * l);
        residuals[i] = q16_from_float(err);
    }
}

delta_kin_result_t delta_forward_kinematics(const delta_cfg_t *cfg,
                                            const q16_16 joint_angles_q16[3],
                                            vec3_q16 *xyz)
{
    vec3_q16 guess = {0};
    guess.z = q16_from_float(-q16_to_float(cfg->L_upper) - q16_to_float(cfg->L_lower));

    for (size_t iter = 0U; iter < 6U; ++iter)
    {
        q16_16 residuals[3];
        forward_equations(cfg, joint_angles_q16, &guess, residuals);
        q16_16 jacobian[9];
        if (delta_jacobian(cfg, joint_angles_q16, jacobian) != DELTA_KIN_OK)
        {
            return DELTA_KIN_SINGULARITY;
        }
        /* Solve J * delta = -residuals using a damped least squares step. */
        vec3_q16 delta = {0};
        for (size_t axis = 0U; axis < 3U; ++axis)
        {
            q16_16 diag = jacobian[axis * 3U + axis];
            if (diag == 0)
            {
                return DELTA_KIN_SINGULARITY;
            }
            q16_16 step = q16_div(residuals[axis], diag);
            if (axis == 0U)
            {
                delta.x = step;
            }
            else if (axis == 1U)
            {
                delta.y = step;
            }
            else
            {
                delta.z = step;
            }
        }
        vec3_sub(&guess, &delta, &guess);
        if (q16_to_int(vec3_norm_sq(&delta)) == 0)
        {
            break;
        }
    }

    if (!delta_point_in_workspace(cfg, &guess))
    {
        return DELTA_KIN_OUT_OF_RANGE;
    }

    *xyz = guess;
    return DELTA_KIN_OK;
}

delta_kin_result_t delta_jacobian(const delta_cfg_t *cfg,
                                  const q16_16 joint_angles_q16[3],
                                  q16_16 jacobian_q16[9])
{
    const float L = q16_to_float(cfg->L_upper);
    for (size_t i = 0U; i < 3U; ++i)
    {
        const float theta = q16_to_float(joint_angles_q16[i]);
        const float vx = -L * sinf(theta);
        const float vy = L * cosf(theta);
        jacobian_q16[i * 3U + 0U] = q16_from_float(vx);
        jacobian_q16[i * 3U + 1U] = q16_from_float(vy);
        jacobian_q16[i * 3U + 2U] = q16_from_float(L);
    }

    q16_16 det = q16_sub(q16_mul(jacobian_q16[0], jacobian_q16[4]),
                         q16_mul(jacobian_q16[1], jacobian_q16[3]));
    if (det == 0)
    {
        return DELTA_KIN_SINGULARITY;
    }
    return DELTA_KIN_OK;
}

bool delta_point_in_workspace(const delta_cfg_t *cfg, const vec3_q16 *xyz)
{
    if (xyz->x < cfg->soft_xyz_min[0] || xyz->x > cfg->soft_xyz_max[0])
    {
        return false;
    }
    if (xyz->y < cfg->soft_xyz_min[1] || xyz->y > cfg->soft_xyz_max[1])
    {
        return false;
    }
    if (xyz->z < cfg->soft_xyz_min[2] || xyz->z > cfg->soft_xyz_max[2])
    {
        return false;
    }
    return true;
}
