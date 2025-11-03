#ifndef MOTION_H
#define MOTION_H

#include <stdbool.h>

#include "calib/calib.h"
#include "cia402/cia402.h"
#include "ethcat/ethcat.h"
#include "planner/planner.h"

/** \brief Motion controller configuration. */
typedef struct
{
    q16_16 period_ms; /**< Control period in milliseconds. */
} motion_config;

/** \brief Motion controller context. */
typedef struct
{
    motion_config config;       /**< Active configuration. */
    planner_context *planner;   /**< Planner reference. */
    ethcat_master *master;      /**< EtherCAT master. */
    calib_context *calib;       /**< Calibration context. */
    cia402_axis axes[3];        /**< Axis controllers. */
} motion_controller;

void motion_default_config(motion_config *cfg);

bool motion_init(motion_controller *motion,
                 const motion_config *cfg,
                 planner_context *planner,
                 ethcat_master *master,
                 calib_context *calib);

void motion_shutdown(motion_controller *motion);

bool motion_step(motion_controller *motion);

#endif
