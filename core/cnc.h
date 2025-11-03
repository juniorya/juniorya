#ifndef CORE_CNC_H
#define CORE_CNC_H

#include <stdbool.h>

#include "calib/calib.h"
#include "drivers/drivers.h"
#include "ethcat/ethcat.h"
#include "motion/motion.h"
#include "opcua/opcua.h"
#include "planner/planner.h"

/** \brief CNC configuration. */
typedef struct
{
    planner_config planner; /**< Planner configuration. */
} cnc_config;

/** \brief CNC runtime context. */
typedef struct
{
    cnc_config config;        /**< Current configuration. */
    calib_context calibration;/**< Calibration data. */
    drivers_context drivers;  /**< Hardware drivers. */
    planner_context planner;  /**< Planner. */
    ethcat_master master;     /**< EtherCAT master. */
    motion_controller motion; /**< Motion control. */
    opcua_server server;      /**< OPC UA server. */
} cnc_context;

void cnc_default_config(cnc_config *cfg);

bool cnc_init(cnc_context *cnc);

void cnc_shutdown(cnc_context *cnc);

bool cnc_run_cycle(cnc_context *cnc);

#endif
