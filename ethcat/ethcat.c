#include "ethcat/ethcat.h"

#include <string.h>

void ethcat_master_default_config(ethcat_master_config *cfg)
{
    if (cfg == NULL)
    {
        return;
    }
    cfg->sync0_rate_hz = q16_from_int(SYNC0_RATE_HZ);
}

bool ethcat_master_init(ethcat_master *master, const ethcat_master_config *cfg)
{
    if ((master == NULL) || (cfg == NULL))
    {
        return false;
    }
    memset(master->last_setpoint, 0, sizeof(master->last_setpoint));
    master->cfg = *cfg;
    master->operational = true;
    return true;
}

void ethcat_master_shutdown(ethcat_master *master)
{
    if (master != NULL)
    {
        master->operational = false;
    }
}

bool ethcat_master_send_setpoints(ethcat_master *master, const q16_16 joints[3])
{
    if ((master == NULL) || (joints == NULL) || !master->operational)
    {
        return false;
    }
    for (size_t i = 0; i < 3; ++i)
    {
        master->last_setpoint[i] = joints[i];
    }
    return true;
}

void ethcat_master_poll(ethcat_master *master)
{
    (void)master;
}
