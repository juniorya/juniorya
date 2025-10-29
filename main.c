/**
 * @file main.c
 * @brief Entry point for the delta CNC firmware.
 *
 * This module wires the scheduler, communication layers and hardware
 * abstractions together. The executable produced from this translation unit is
 * designed to run both on real-time targets (QNX, VxWorks, Baget) and within a
 * host simulation environment, including QEMU for MIPS64. The control loop
 * operates at a default 1 kHz period, coordinating trajectory planning,
 * kinematic evaluation and EtherCAT fieldbus communication.
 */

#include "core/cnc.h"
#include "drivers/drivers.h"
#include "ethcat/ethcat.h"
#include "motion/motion.h"
#include "osal/osal.h"
#include "planner/planner.h"
#include "storage/storage.h"

#include <stdio.h>

/**
 * @brief Application entry point.
 *
 * @param argc Command line argument count.
 * @param argv Command line argument vector.
 * @return Zero on successful termination, non-zero on fatal error.
 */
int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    if (!osal_init())
    {
        fprintf(stderr, "OSAL initialisation failed\n");
        return 1;
    }

    storage_init_options storage_opts;
    storage_default_options(&storage_opts);
    if (!storage_init(&storage_opts))
    {
        fprintf(stderr, "Storage initialisation failed\n");
        return 1;
    }

    drivers_context drivers;
    if (!drivers_init(&drivers))
    {
        fprintf(stderr, "Drivers initialisation failed\n");
        return 1;
    }

    ethcat_master master;
    ethcat_master_config master_cfg;
    ethcat_master_default_config(&master_cfg);
    if (!ethcat_master_init(&master, &master_cfg))
    {
        fprintf(stderr, "EtherCAT master initialisation failed\n");
        return 1;
    }

    planner_context planner;
    planner_config planner_cfg;
    planner_default_config(&planner_cfg);
    if (!planner_init(&planner, &planner_cfg))
    {
        fprintf(stderr, "Planner initialisation failed\n");
        return 1;
    }

    motion_controller motion;
    motion_config motion_cfg;
    motion_default_config(&motion_cfg);
    if (!motion_init(&motion, &motion_cfg, &planner, &master))
    {
        fprintf(stderr, "Motion controller initialisation failed\n");
        return 1;
    }

    cnc_context cnc;
    cnc_config cnc_cfg;
    cnc_default_config(&cnc_cfg);
    if (!cnc_init(&cnc, &cnc_cfg, &drivers, &planner, &motion, &master))
    {
        fprintf(stderr, "CNC initialisation failed\n");
        return 1;
    }

    cnc_run(&cnc);

    cnc_shutdown(&cnc);
    motion_shutdown(&motion);
    planner_shutdown(&planner);
    ethcat_master_shutdown(&master);
    drivers_shutdown(&drivers);
    storage_shutdown();
    osal_shutdown();
    return 0;
}
