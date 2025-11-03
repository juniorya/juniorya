#include "kinematics/delta.h"

#include "utils/q16.h"

#include <string.h>

void delta_default_config(delta_cfg_t *cfg)
{
    if (cfg == NULL)
    {
        return;
    }
    memset(cfg, 0, sizeof(*cfg));
    cfg->R_base = q16_from_int(150);
    cfg->r_eff = q16_from_int(50);
    cfg->L_upper = q16_from_int(250);
    cfg->L_lower = q16_from_int(500);
    cfg->z_offset = q16_from_int(-300);
    cfg->soft_xyz_min[0] = q16_from_int(-200);
    cfg->soft_xyz_min[1] = q16_from_int(-200);
    cfg->soft_xyz_min[2] = q16_from_int(-600);
    cfg->soft_xyz_max[0] = q16_from_int(200);
    cfg->soft_xyz_max[1] = q16_from_int(200);
    cfg->soft_xyz_max[2] = q16_from_int(-100);
    cfg->scale_per_axis[0] = q16_from_int(1);
    cfg->scale_per_axis[1] = q16_from_int(1);
    cfg->scale_per_axis[2] = q16_from_int(1);
}

bool delta_is_within_workspace(const delta_cfg_t *cfg, vec3_q16 xyz)
{
    if (cfg == NULL)
    {
        return false;
    }
    if ((xyz.x < cfg->soft_xyz_min[0]) || (xyz.x > cfg->soft_xyz_max[0]))
    {
        return false;
    }
    if ((xyz.y < cfg->soft_xyz_min[1]) || (xyz.y > cfg->soft_xyz_max[1]))
    {
        return false;
    }
    if ((xyz.z < cfg->soft_xyz_min[2]) || (xyz.z > cfg->soft_xyz_max[2]))
    {
        return false;
    }
    return true;
}

bool delta_inverse(const delta_cfg_t *cfg, vec3_q16 xyz, q16_16 joints[3])
{
    if ((cfg == NULL) || (joints == NULL))
    {
        return false;
    }
    if (!delta_is_within_workspace(cfg, xyz))
    {
        return false;
    }
    joints[0] = cfg->calib_offsets[0] + q16_mul(cfg->scale_per_axis[0], xyz.x);
    joints[1] = cfg->calib_offsets[1] + q16_mul(cfg->scale_per_axis[1], xyz.y);
    joints[2] = cfg->calib_offsets[2] + q16_mul(cfg->scale_per_axis[2], xyz.z - cfg->z_offset);
    return true;
}

bool delta_forward(const delta_cfg_t *cfg, const q16_16 joints[3], vec3_q16 *xyz)
{
    if ((cfg == NULL) || (joints == NULL) || (xyz == NULL))
    {
        return false;
    }
    if ((cfg->scale_per_axis[0] == 0) || (cfg->scale_per_axis[1] == 0) || (cfg->scale_per_axis[2] == 0))
    {
        return false;
    }
    xyz->x = q16_div(joints[0] - cfg->calib_offsets[0], cfg->scale_per_axis[0]);
    xyz->y = q16_div(joints[1] - cfg->calib_offsets[1], cfg->scale_per_axis[1]);
    xyz->z = q16_div(joints[2] - cfg->calib_offsets[2], cfg->scale_per_axis[2]) + cfg->z_offset;
    return delta_is_within_workspace(cfg, *xyz);
}

q16_16 delta_jacobian_det(const delta_cfg_t *cfg, const q16_16 joints[3])
{
    (void)cfg;
    if (joints == NULL)
    {
        return 0;
    }
    q16_16 diff1 = joints[0] - joints[1];
    q16_16 diff2 = joints[1] - joints[2];
    q16_16 diff3 = joints[2] - joints[0];
    q16_16 sum = q16_mul(diff1, diff2) + q16_mul(diff2, diff3);
    return q16_abs(sum);
}
