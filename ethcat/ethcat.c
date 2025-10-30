/**
 * @file ethcat.c
 * @brief EtherCAT master simulation implementing Sync0 scheduling.
 */

#include "ethcat.h"

#include <string.h>

#include "log.h"
#include "osal/osal.h"

void ethcat_master_default_config(ethcat_master_config *cfg)
{
    (void)memset(cfg, 0, sizeof(*cfg));
    cfg->sync_period_us = q16_from_int(1000);
    cfg->axis_count = 3U;
    cfg->drive_count = 0U;
}

bool ethcat_master_init(ethcat_master *master, const ethcat_master_config *cfg)
{
    master->config = *cfg;
    for (size_t i = 0; i < ETHCAT_MAX_AXES; ++i)
    {
        master->mode[i] = CIA402_MODE_CSP;
        master->target_pos[i] = 0;
        master->target_vel[i] = 0;
        master->target_torque[i] = 0;
        master->actual_pos[i] = 0;
        master->actual_vel[i] = 0;
        master->actual_torque[i] = 0;
    }
    if (cfg->drive_count > 0U)
    {
        log_printf(LOG_INFO, "EtherCAT master initialised with %u axes using drive %s",
                   (unsigned)cfg->axis_count,
                   cfg->drives[0].name);
    }
    else
    {
        log_printf(LOG_INFO, "EtherCAT master initialised with %u axes", (unsigned)cfg->axis_count);
    }
    return true;
}

void ethcat_master_update(ethcat_master *master)
{
    const q16_16 smoothing = q16_from_int(4);
    for (size_t i = 0; i < master->config.axis_count && i < ETHCAT_MAX_AXES; ++i)
    {
        switch (master->mode[i])
        {
            case CIA402_MODE_PP:
            case CIA402_MODE_IP:
            case CIA402_MODE_CSP:
            {
                q16_16 error = q16_sub(master->target_pos[i], master->actual_pos[i]);
                q16_16 step = q16_div(error, smoothing, NULL);
                master->actual_vel[i] = q16_add(step, master->target_vel[i]);
                master->actual_pos[i] = q16_add(master->actual_pos[i], step);
                master->actual_torque[i] = master->target_torque[i];
                break;
            }
            case CIA402_MODE_PV:
            case CIA402_MODE_CSV:
            {
                q16_16 vel_err = q16_sub(master->target_vel[i], master->actual_vel[i]);
                q16_16 vel_step = q16_div(vel_err, smoothing, NULL);
                master->actual_vel[i] = q16_add(master->actual_vel[i], vel_step);
                master->actual_pos[i] = q16_add(master->actual_pos[i], master->actual_vel[i]);
                master->actual_torque[i] = master->target_torque[i];
                break;
            }
            case CIA402_MODE_PT:
            case CIA402_MODE_CST:
            {
                q16_16 torque_err = q16_sub(master->target_torque[i], master->actual_torque[i]);
                q16_16 torque_step = q16_div(torque_err, smoothing, NULL);
                master->actual_torque[i] = q16_add(master->actual_torque[i], torque_step);
                master->actual_vel[i] = q16_add(master->actual_vel[i], master->actual_torque[i]);
                master->actual_pos[i] = q16_add(master->actual_pos[i], master->actual_vel[i]);
                break;
            }
            case CIA402_MODE_HM:
            {
                q16_16 error = q16_sub(0, master->actual_pos[i]);
                q16_16 step = q16_div(error, smoothing, NULL);
                master->actual_vel[i] = step;
                master->actual_pos[i] = q16_add(master->actual_pos[i], step);
                master->actual_torque[i] = master->target_torque[i];
                if (q16_abs(master->actual_pos[i]) < q16_from_float(0.01f))
                {
                    master->target_pos[i] = 0;
                }
                break;
            }
            case CIA402_MODE_NONE:
            default:
            {
                break;
            }
        }
    }
    osal_sleep_ms(q16_to_int(master->config.sync_period_us));
}

void ethcat_master_set_operation_mode(ethcat_master *master, size_t axis, cia402_mode mode)
{
    if (axis < ETHCAT_MAX_AXES)
    {
        master->mode[axis] = mode;
    }
}

void ethcat_master_set_position(ethcat_master *master, size_t axis, q16_16 target)
{
    if (axis < ETHCAT_MAX_AXES)
    {
        master->target_pos[axis] = target;
    }
}

void ethcat_master_set_velocity(ethcat_master *master, size_t axis, q16_16 target)
{
    if (axis < ETHCAT_MAX_AXES)
    {
        master->target_vel[axis] = target;
    }
}

void ethcat_master_set_torque(ethcat_master *master, size_t axis, q16_16 target)
{
    if (axis < ETHCAT_MAX_AXES)
    {
        master->target_torque[axis] = target;
    }
}

void ethcat_master_set_target(ethcat_master *master, size_t axis, q16_16 target)
{
    ethcat_master_set_position(master, axis, target);
}

q16_16 ethcat_master_get_actual_position(const ethcat_master *master, size_t axis)
{
    if (axis < ETHCAT_MAX_AXES)
    {
        return master->actual_pos[axis];
    }
    return 0;
}

q16_16 ethcat_master_get_actual_velocity(const ethcat_master *master, size_t axis)
{
    if (axis < ETHCAT_MAX_AXES)
    {
        return master->actual_vel[axis];
    }
    return 0;
}

q16_16 ethcat_master_get_actual_torque(const ethcat_master *master, size_t axis)
{
    if (axis < ETHCAT_MAX_AXES)
    {
        return master->actual_torque[axis];
    }
    return 0;
}

void ethcat_master_shutdown(ethcat_master *master)
{
    (void)master;
    log_printf(LOG_INFO, "EtherCAT master shutdown");
}
