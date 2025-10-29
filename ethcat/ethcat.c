/**
 * @file ethcat.c
 * @brief EtherCAT master simulation implementing Sync0 scheduling.
 */

#include "ethcat.h"

#include "log.h"
#include "osal/osal.h"

void ethcat_master_default_config(ethcat_master_config *cfg)
{
    cfg->sync_period_us = q16_from_int(1000);
    cfg->axis_count = 3U;
}

bool ethcat_master_init(ethcat_master *master, const ethcat_master_config *cfg)
{
    master->config = *cfg;
    for (size_t i = 0; i < master->config.axis_count; ++i)
    {
        master->target_pos[i] = 0;
        master->actual_pos[i] = 0;
    }
    log_printf(LOG_INFO, "EtherCAT master initialised with %u axes", (unsigned)cfg->axis_count);
    return true;
}

void ethcat_master_update(ethcat_master *master)
{
    for (size_t i = 0; i < master->config.axis_count; ++i)
    {
        q16_16 error = q16_sub(master->target_pos[i], master->actual_pos[i]);
        master->actual_pos[i] = q16_add(master->actual_pos[i], q16_div(error, q16_from_int(4), NULL));
    }
    osal_sleep_ms(q16_to_int(master->config.sync_period_us));
}

void ethcat_master_set_target(ethcat_master *master, size_t axis, q16_16 target)
{
    if (axis < master->config.axis_count)
    {
        master->target_pos[axis] = target;
    }
}

void ethcat_master_shutdown(ethcat_master *master)
{
    (void)master;
    log_printf(LOG_INFO, "EtherCAT master shutdown");
}
