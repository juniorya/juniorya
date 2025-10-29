/**
 * @file delta_kinematics.c
 * @brief Analytical and numerical kinematics for delta robot.
 */

#include "delta_kinematics.h"
#include "utils/vec3.h"

#include <math.h>
#include <string.h>

static float q(q16_16 v)
{
    return q16_to_float(v);
}

static q16_16 fq(float v)
{
    return q16_from_float(v);
}

static bool solve_single_arm(float x, float y, float z,
                             float base_radius,
                             float eff_radius,
                             float upper,
                             float lower,
                             float *theta)
{
    const float tan30 = 1.0f / sqrtf(3.0f);
    const float y1 = -0.5f * tan30 * base_radius;
    float y0 = y - 0.5f * tan30 * eff_radius;
    float a = (x * x + y0 * y0 + z * z + upper * upper - lower * lower -
               base_radius * base_radius * 0.25f - y1 * y1) / (2.0f * z);
    float b = (y1 - y0) / z;
    float disc = upper * upper * (b * b + 1.0f) - (a + b * y1) * (a + b * y1);
    if (disc < 0.0f)
    {
        return false;
    }
    float yj = (y1 - a * b - sqrtf(disc)) / (b * b + 1.0f);
    float zj = a + b * yj;
    *theta = atan2f(-zj, y1 - yj);
    return true;
}

static void rotate120(float *x, float *y)
{
    const float cos120 = -0.5f;
    const float sin120 = 0.8660254037844386f;
    float rx = *x * cos120 - *y * sin120;
    float ry = *x * sin120 + *y * cos120;
    *x = rx;
    *y = ry;
}

bool delta_inverse_kinematics(const delta_cfg_t *cfg, const vec3_q16 *position, q16_16 theta[3])
{
    float x = q(position->x);
    float y = q(position->y);
    float z = q(position->z) - q(cfg->z_offset);
    float base = q(cfg->R_base);
    float eff = q(cfg->r_eff);
    float upper = q(cfg->L_upper);
    float lower = q(cfg->L_lower);

    float theta0;
    if (!solve_single_arm(x, y, z, base, eff, upper, lower, &theta0))
    {
        return false;
    }
    float x1 = x;
    float y1 = y;
    rotate120(&x1, &y1);
    float theta1;
    if (!solve_single_arm(x1, y1, z, base, eff, upper, lower, &theta1))
    {
        return false;
    }
    rotate120(&x1, &y1);
    float theta2;
    if (!solve_single_arm(x1, y1, z, base, eff, upper, lower, &theta2))
    {
        return false;
    }

    theta[0] = fq(theta0);
    theta[1] = fq(theta1);
    theta[2] = fq(theta2);
    return true;
}

bool delta_forward_kinematics(const delta_cfg_t *cfg, const q16_16 theta[3], vec3_q16 *position)
{
    vec3_q16 guess = vec3_from_float(0.0f, 0.0f, q(cfg->z_offset));
    for (int iter = 0; iter < 20; ++iter)
    {
        q16_16 ik[3];
        if (!delta_inverse_kinematics(cfg, &guess, ik))
        {
            return false;
        }
        float err0 = q16_to_float(q16_sub(ik[0], theta[0]));
        float err1 = q16_to_float(q16_sub(ik[1], theta[1]));
        float err2 = q16_to_float(q16_sub(ik[2], theta[2]));
        float norm = fabsf(err0) + fabsf(err1) + fabsf(err2);
        if (norm < 1e-4f)
        {
            *position = guess;
            return true;
        }
        float J[3][3];
        const float delta = 1e-3f;
        for (int axis = 0; axis < 3; ++axis)
        {
            vec3_q16 pert = guess;
            if (axis == 0)
            {
                pert.x = q16_add(pert.x, fq(delta));
            }
            else if (axis == 1)
            {
                pert.y = q16_add(pert.y, fq(delta));
            }
            else
            {
                pert.z = q16_add(pert.z, fq(delta));
            }
            q16_16 ikp[3];
            if (!delta_inverse_kinematics(cfg, &pert, ikp))
            {
                return false;
            }
            J[0][axis] = (q16_to_float(ikp[0]) - q16_to_float(ik[0])) / delta;
            J[1][axis] = (q16_to_float(ikp[1]) - q16_to_float(ik[1])) / delta;
            J[2][axis] = (q16_to_float(ikp[2]) - q16_to_float(ik[2])) / delta;
        }
        float det = J[0][0] * (J[1][1] * J[2][2] - J[1][2] * J[2][1]) -
                    J[0][1] * (J[1][0] * J[2][2] - J[1][2] * J[2][0]) +
                    J[0][2] * (J[1][0] * J[2][1] - J[1][1] * J[2][0]);
        if (fabsf(det) < 1e-6f)
        {
            return false;
        }
        float inv[3][3];
        inv[0][0] = (J[1][1] * J[2][2] - J[1][2] * J[2][1]) / det;
        inv[0][1] = (J[0][2] * J[2][1] - J[0][1] * J[2][2]) / det;
        inv[0][2] = (J[0][1] * J[1][2] - J[0][2] * J[1][1]) / det;
        inv[1][0] = (J[1][2] * J[2][0] - J[1][0] * J[2][2]) / det;
        inv[1][1] = (J[0][0] * J[2][2] - J[0][2] * J[2][0]) / det;
        inv[1][2] = (J[0][2] * J[1][0] - J[0][0] * J[1][2]) / det;
        inv[2][0] = (J[1][0] * J[2][1] - J[1][1] * J[2][0]) / det;
        inv[2][1] = (J[0][1] * J[2][0] - J[0][0] * J[2][1]) / det;
        inv[2][2] = (J[0][0] * J[1][1] - J[0][1] * J[1][0]) / det;
        float dx = -(inv[0][0] * err0 + inv[0][1] * err1 + inv[0][2] * err2);
        float dy = -(inv[1][0] * err0 + inv[1][1] * err1 + inv[1][2] * err2);
        float dz = -(inv[2][0] * err0 + inv[2][1] * err1 + inv[2][2] * err2);
        guess.x = q16_add(guess.x, fq(dx));
        guess.y = q16_add(guess.y, fq(dy));
        guess.z = q16_add(guess.z, fq(dz));
    }
    return false;
}

bool delta_jacobian(const delta_cfg_t *cfg, const q16_16 theta[3], matrix3_q16 *jacobian)
{
    vec3_q16 p;
    if (!delta_forward_kinematics(cfg, theta, &p))
    {
        return false;
    }
    const float delta = 1e-3f;
    for (int axis = 0; axis < 3; ++axis)
    {
        q16_16 perturbed[3] = {theta[0], theta[1], theta[2]};
        perturbed[axis] = q16_add(perturbed[axis], fq(delta));
        vec3_q16 p2;
        if (!delta_forward_kinematics(cfg, perturbed, &p2))
        {
            return false;
        }
        jacobian->m[0][axis] = q16_div(q16_sub(p2.x, p.x), fq(delta), NULL);
        jacobian->m[1][axis] = q16_div(q16_sub(p2.y, p.y), fq(delta), NULL);
        jacobian->m[2][axis] = q16_div(q16_sub(p2.z, p.z), fq(delta), NULL);
    }
    return true;
}

bool delta_within_workspace(const delta_cfg_t *cfg, const vec3_q16 *position)
{
    float x = q(position->x);
    float y = q(position->y);
    float z = q(position->z);
    return (x >= q(cfg->soft_xyz_min[0]) && x <= q(cfg->soft_xyz_max[0]) &&
            y >= q(cfg->soft_xyz_min[1]) && y <= q(cfg->soft_xyz_max[1]) &&
            z >= q(cfg->soft_xyz_min[2]) && z <= q(cfg->soft_xyz_max[2]));
}
