#include "core/cnc.h"

#include "storage/storage.h"
#include "utils/q16.h"
#include "kinematics/delta.h"

void cnc_default_config(cnc_config *cfg)
{
    if (cfg != NULL)
    {
        planner_default_config(&cfg->planner);
    }
}

bool cnc_init(cnc_context *cnc)
{
    if (cnc == NULL)
    {
        return false;
    }
    cnc_default_config(&cnc->config);
    delta_cfg_t defaults;
    delta_default_config(&defaults);
    calib_init(&cnc->calibration, &defaults);

    storage_options opts;
    storage_default_options(&opts);
    storage_init(&opts);
    calib_load(&cnc->calibration);

    drivers_init(&cnc->drivers);

    planner_init(&cnc->planner, &cnc->config.planner);

    ethcat_master_config master_cfg;
    ethcat_master_default_config(&master_cfg);
    ethcat_master_init(&cnc->master, &master_cfg);

    motion_config motion_cfg;
    motion_default_config(&motion_cfg);
    motion_init(&cnc->motion, &motion_cfg, &cnc->planner, &cnc->master, &cnc->calibration);

    opcua_server_init(&cnc->server);
    return true;
}

void cnc_shutdown(cnc_context *cnc)
{
    if (cnc == NULL)
    {
        return;
    }
    opcua_server_shutdown(&cnc->server);
    motion_shutdown(&cnc->motion);
    ethcat_master_shutdown(&cnc->master);
    drivers_shutdown(&cnc->drivers);
    storage_shutdown();
}

bool cnc_run_cycle(cnc_context *cnc)
{
    if (cnc == NULL)
    {
        return false;
    }
    if (!motion_step(&cnc->motion))
    {
        return false;
    }
    opcua_server_publish_pose(&cnc->server, cnc->planner.current_position);
    ethcat_master_poll(&cnc->master);
    return true;
}
