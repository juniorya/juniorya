/**
 * @file cnc.h
 * @brief High-level CNC controller orchestrating planner, motion and fieldbus.
 */

#ifndef CNC_H
#define CNC_H

#include <stdbool.h>
#include <stddef.h>

#include "cia402/cia402.h"
#include "drivers/drivers.h"
#include "ethcat/ethcat.h"
#include "gcode/gcode.h"
#include "motion/motion.h"
#include "planner/planner.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief CNC state enumeration. */
typedef enum
{
    CNC_STATE_IDLE = 0, /**< Controller is idle. */
    CNC_STATE_RUNNING,  /**< Executing programmed trajectory. */
    CNC_STATE_HOLD,     /**< Hold due to operator request. */
    CNC_STATE_FAULT     /**< Faulted due to alarm condition. */
} cnc_state;

/** @brief CNC configuration parameters. */
typedef struct
{
    uint32_t control_period_us; /**< Control loop period in microseconds. */
    bool enable_g5;             /**< Enable Bézier/NURBS commands. */
} cnc_config;

struct cnc_context;

/** @brief Runtime CNC controller instance. */
typedef struct cnc_context
{
    cnc_config config;             /**< Static configuration. */
    cnc_state state;               /**< Current CNC state. */
    drivers_context *drivers;      /**< Hardware driver hooks. */
    planner_context *planner;      /**< Motion planner context. */
    motion_controller *motion;     /**< Motion control loop. */
    ethcat_master *master;         /**< EtherCAT master. */
    cia402_axis axis[3];           /**< Axes implementing CiA-402 profile. */
    gcode_parser parser;           /**< G-code parser state. */
} cnc_context;

/**
 * @brief Initialise CNC configuration structure with defaults.
 * @param cfg Configuration structure to populate.
 */
void cnc_default_config(cnc_config *cfg);

/**
 * @brief Initialise CNC controller runtime.
 * @param cnc CNC context to initialise.
 * @param cfg Configuration parameters.
 * @param drivers Hardware drivers instance.
 * @param planner Planner instance.
 * @param motion Motion controller.
 * @param master EtherCAT master instance.
 * @return True on success.
 */
bool cnc_init(cnc_context *cnc,
              const cnc_config *cfg,
              drivers_context *drivers,
              planner_context *planner,
              motion_controller *motion,
              ethcat_master *master);

/**
 * @brief Run CNC main loop.
 * @param cnc CNC instance.
 */
void cnc_run(cnc_context *cnc);

/**
 * @brief Request CNC hold state.
 * @param cnc CNC instance.
 */
void cnc_request_hold(cnc_context *cnc);

/**
 * @brief Reset faults and return to idle.
 * @param cnc CNC instance.
 */
void cnc_reset_fault(cnc_context *cnc);

/**
 * @brief Shut down CNC controller.
 * @param cnc CNC instance.
 */
void cnc_shutdown(cnc_context *cnc);

#ifdef __cplusplus
}
#endif

#endif
