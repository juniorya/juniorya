/**
 * @file board.c
 * @brief Board-specific configuration defaults.
 */

#include "board.h"
#include "utils/q16.h"

void board_default_delta_config(delta_cfg_t *cfg)
{
    cfg->R_base = q16_from_float(150.0f);
    cfg->r_eff = q16_from_float(50.0f);
    cfg->L_upper = q16_from_float(300.0f);
    cfg->L_lower = q16_from_float(800.0f);
    cfg->z_offset = q16_from_float(400.0f);
    for (int i = 0; i < 3; ++i)
    {
        cfg->soft_xyz_min[i] = q16_from_float(-200.0f);
        cfg->soft_xyz_max[i] = q16_from_float(200.0f);
        cfg->calib_offsets[i] = 0;
        cfg->scale_per_axis[i] = q16_from_float(1.0f);
    }
}
