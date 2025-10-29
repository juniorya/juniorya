#include "delta.h"
#include <math.h>

static delta_cfg_t s_cfg;
static delta_pose_t s_last_pose;

#define TAN30 0.5773502691896258f
#define SIN120 0.8660254037844386f
#define COS120 -0.5f

static float q_to_float(q16_16_t v)
{
    return q16_16_to_float(v);
}

static q16_16_t float_to_q(float v)
{
    return q16_16_from_float(v);
}

void delta_init(const delta_cfg_t *cfg)
{
    s_cfg = *cfg;
    for (int i = 0; i < 3; ++i) {
        s_last_pose.xyz[i] = 0;
    }
}

static bool delta_calc_angle(float x0, float y0, float z0, float *theta)
{
    float e = q_to_float(s_cfg.r_eff) * 2.0f;
    float f = q_to_float(s_cfg.R_base) * 2.0f;
    float re = q_to_float(s_cfg.L_lower);
    float rf = q_to_float(s_cfg.L_upper);

    float y1 = -0.5f * TAN30 * f;
    y0 -= 0.5f * TAN30 * e;

    float a = (x0 * x0 + y0 * y0 + z0 * z0 + rf * rf - re * re - y1 * y1) / (2.0f * z0);
    float b = (y1 - y0) / z0;
    float discr = rf * (rf * (b * b + 1.0f)) - (a + b * y1) * (a + b * y1);
    if (discr < 0.0f) {
        return false;
    }

    float yj = (y1 - a * b - sqrtf(discr)) / (b * b + 1.0f);
    float zj = a + b * yj;
    *theta = atanf(-zj / (y1 - yj));
    return true;
}

bool delta_inverse_kinematics(const delta_pose_t *cart, delta_joint_t *joints)
{
    if (!delta_within_workspace(cart)) {
        return false;
    }

    float x = q_to_float(cart->xyz[0]);
    float y = q_to_float(cart->xyz[1]);
    float z = q_to_float(cart->xyz[2]) + q_to_float(s_cfg.z_offset);

    float theta0, theta1, theta2;
    if (!delta_calc_angle(x, y, z, &theta0)) {
        return false;
    }
    float x1 = x * COS120 + y * SIN120;
    float y1 = -x * SIN120 + y * COS120;
    if (!delta_calc_angle(x1, y1, z, &theta1)) {
        return false;
    }
    float x2 = x * COS120 - y * SIN120;
    float y2 = x * SIN120 + y * COS120;
    if (!delta_calc_angle(x2, y2, z, &theta2)) {
        return false;
    }

    joints->theta[0] = float_to_q(theta0);
    joints->theta[1] = float_to_q(theta1);
    joints->theta[2] = float_to_q(theta2);
    s_last_pose = *cart;
    return true;
}



static bool solve_linear3(float A[3][3], const float b[3], float x[3])
{
    float m[3][4];
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            m[i][j] = A[i][j];
        }
        m[i][3] = -b[i];
    }
    for (int i = 0; i < 3; ++i) {
        int pivot = i;
        for (int r = i + 1; r < 3; ++r) {
            if (fabsf(m[r][i]) > fabsf(m[pivot][i])) {
                pivot = r;
            }
        }
        if (fabsf(m[pivot][i]) < 1e-9f) {
            return false;
        }
        if (pivot != i) {
            for (int c = i; c < 4; ++c) {
                float tmp = m[i][c];
                m[i][c] = m[pivot][c];
                m[pivot][c] = tmp;
            }
        }
        float inv = 1.0f / m[i][i];
        for (int c = i; c < 4; ++c) {
            m[i][c] *= inv;
        }
        for (int r = 0; r < 3; ++r) {
            if (r == i) {
                continue;
            }
            float factor = m[r][i];
            for (int c = i; c < 4; ++c) {
                m[r][c] -= factor * m[i][c];
            }
        }
    }
    for (int i = 0; i < 3; ++i) {
        x[i] = m[i][3];
    }
    return true;
}



bool delta_forward_kinematics(const delta_joint_t *joints, delta_pose_t *cart)
{
    delta_pose_t pose = s_last_pose;
    for (int i = 0; i < 3; ++i) {
        if (pose.xyz[i] < s_cfg.soft_xyz_min[i] || pose.xyz[i] > s_cfg.soft_xyz_max[i]) {
            pose.xyz[i] = (s_cfg.soft_xyz_min[i] + s_cfg.soft_xyz_max[i]) / 2;
        }
    }
    const float epsilon = 0.0005f;
    for (int iter = 0; iter < 20; ++iter) {
        delta_joint_t estimate;
        if (!delta_inverse_kinematics(&pose, &estimate)) {
            return false;
        }
        float error[3];
        float max_error = 0.0f;
        for (int axis = 0; axis < 3; ++axis) {
            error[axis] = q16_16_to_float(estimate.theta[axis] - joints->theta[axis]);
            if (fabsf(error[axis]) > max_error) {
                max_error = fabsf(error[axis]);
            }
        }
        if (max_error < 1e-4f) {
            *cart = pose;
            return true;
        }
        float J[3][3];
        for (int i = 0; i < 3; ++i) {
            delta_pose_t perturbed = pose;
            perturbed.xyz[i] += float_to_q(epsilon);
            delta_joint_t perturbed_j;
            if (!delta_inverse_kinematics(&perturbed, &perturbed_j)) {
                return false;
            }
            for (int axis = 0; axis < 3; ++axis) {
                float diff = q16_16_to_float(perturbed_j.theta[axis] - estimate.theta[axis]);
                J[axis][i] = diff / epsilon;
            }
        }
        float delta_xyz[3];
        if (!solve_linear3(J, error, delta_xyz)) {
            return false;
        }
        for (int axis = 0; axis < 3; ++axis) {
            pose.xyz[axis] += float_to_q(delta_xyz[axis]);
        }
    }
    return false;
}
bool delta_within_workspace(const delta_pose_t *cart)
{
    for (int i = 0; i < 3; ++i) {
        if (cart->xyz[i] < s_cfg.soft_xyz_min[i] || cart->xyz[i] > s_cfg.soft_xyz_max[i]) {
            return false;
        }
    }
    return true;
}

void delta_compute_jacobian(const delta_joint_t *joints, delta_jacobian_t *out)
{
    const float delta_theta = 0.0005f;
    delta_pose_t base_pose;
    if (!delta_forward_kinematics(joints, &base_pose)) {
        out->singular = true;
        return;
    }
    float base[3] = {
        q_to_float(base_pose.xyz[0]),
        q_to_float(base_pose.xyz[1]),
        q_to_float(base_pose.xyz[2])
    };

    for (int j = 0; j < 3; ++j) {
        delta_joint_t perturbed = *joints;
        perturbed.theta[j] = float_to_q(q_to_float(perturbed.theta[j]) + delta_theta);
        delta_pose_t pose;
        if (!delta_forward_kinematics(&perturbed, &pose)) {
            out->singular = true;
            return;
        }
        out->jacobian.data[0][j] = float_to_q((q_to_float(pose.xyz[0]) - base[0]) / delta_theta);
        out->jacobian.data[1][j] = float_to_q((q_to_float(pose.xyz[1]) - base[1]) / delta_theta);
        out->jacobian.data[2][j] = float_to_q((q_to_float(pose.xyz[2]) - base[2]) / delta_theta);
    }
    out->det = mat3x3_determinant(&out->jacobian);
    out->singular = q16_16_abs(out->det) < float_to_q(0.0001f);
}
