/**
 * @file main.c
 * @brief Entry point for the delta CNC firmware with process isolation.
 */

#include "core/cnc.h"
#include "drivers/drivers.h"
#include "ethcat/ethcat.h"
#include "motion/motion.h"
#include "opcua/opcua.h"
#include "osal/osal.h"
#include "planner/planner.h"
#include "storage/storage.h"

#include <stdio.h>
#include <string.h>

/**
 * @brief Aggregated configuration passed to child processes.
 */
typedef struct
{
    storage_init_options storage_opts; /**< Non-volatile storage options. */
    planner_config planner_cfg;        /**< Planner baseline configuration. */
} main_launch_config;

static void control_process_entry(void *arg)
{
    main_launch_config cfg = *(main_launch_config *)arg;

    storage_init_options storage_opts = cfg.storage_opts;
    if (!storage_init(&storage_opts))
    {
        fprintf(stderr, "[RT] Storage initialisation failed\n");
        return;
    }

    drivers_context drivers;
    if (!drivers_init(&drivers))
    {
        fprintf(stderr, "[RT] Drivers initialisation failed\n");
        return;
    }

    ethcat_master master;
    ethcat_master_config master_cfg;
    ethcat_master_default_config(&master_cfg);
    if (!ethcat_master_init(&master, &master_cfg))
    {
        fprintf(stderr, "[RT] EtherCAT master initialisation failed\n");
        return;
    }

    planner_context planner;
    if (!planner_init(&planner, &cfg.planner_cfg))
    {
        fprintf(stderr, "[RT] Planner initialisation failed\n");
        return;
    }

    motion_controller motion;
    motion_config motion_cfg;
    motion_default_config(&motion_cfg);
    if (!motion_init(&motion, &motion_cfg, &planner, &master))
    {
        fprintf(stderr, "[RT] Motion controller initialisation failed\n");
        return;
    }

    cnc_context cnc;
    cnc_config cnc_cfg;
    cnc_default_config(&cnc_cfg);
    if (!cnc_init(&cnc, &cnc_cfg, &drivers, &planner, &motion, &master))
    {
        fprintf(stderr, "[RT] CNC init failed\n");
        return;
    }

    cnc_run(&cnc);

    cnc_shutdown(&cnc);
    motion_shutdown(&motion);
    planner_shutdown(&planner);
    ethcat_master_shutdown(&master);
    drivers_shutdown(&drivers);
    storage_shutdown();
}

static void auxiliary_process_entry(void *arg)
{
    (void)arg;
    opcua_server server;
    (void)opcua_server_init(&server, NULL);
    for (int i = 0; i < 50; ++i)
    {
        opcua_server_step(&server);
        osal_sleep_ms(100U);
    }
    opcua_server_shutdown(&server);
}

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    if (!osal_init())
    {
        fprintf(stderr, "OSAL initialisation failed\n");
        return 1;
    }

    main_launch_config launch_cfg;
    storage_default_options(&launch_cfg.storage_opts);
    planner_default_config(&launch_cfg.planner_cfg);

    osal_process *rt = osal_process_spawn(control_process_entry,
                                          &launch_cfg,
                                          OSAL_PROCESS_CLASS_REALTIME);
    if (rt == NULL)
    {
        fprintf(stderr, "Failed to spawn real-time process\n");
        osal_shutdown();
        return 1;
    }

    osal_process *aux = osal_process_spawn(auxiliary_process_entry,
                                           &launch_cfg,
                                           OSAL_PROCESS_CLASS_AUXILIARY);
    if (aux == NULL)
    {
        fprintf(stderr, "Failed to spawn auxiliary process\n");
        osal_process_wait(rt);
        osal_process_destroy(rt);
        osal_shutdown();
        return 1;
    }

    osal_process_wait(rt);
    osal_process_wait(aux);
    osal_process_destroy(rt);
    osal_process_destroy(aux);

    osal_shutdown();
    return 0;
}
