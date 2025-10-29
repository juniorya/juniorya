/**
 * @file motion.h
 * @brief Motion execution and Sync0 synchronization helpers.
 */
#ifndef MOTION_MOTION_H
#define MOTION_MOTION_H

#include <stdbool.h>

#include "planner/planner.h"
#include "ethcat/ethcat.h"
#include "cia402/cia402.h"

/**
 * @brief Motion controller context.
 */
typedef struct
{
    planner_t *planner; /**< Planner instance. */
    ethcat_master_t *master; /**< EtherCAT master. */
    cia402_axis_t axes[3]; /**< CiA-402 states. */
} motion_controller_t;

/**
 * @brief Initialize motion controller.
 *
 * @param motion Motion controller context.
 * @param planner Planner instance.
 * @param master EtherCAT master.
 */
void motion_init(motion_controller_t *motion,
                 planner_t *planner,
                 ethcat_master_t *master);

/**
 * @brief Process a Sync0 tick.
 *
 * @param motion Motion controller context.
 * @param period_ms Control period.
 */
void motion_sync0_tick(motion_controller_t *motion, q16_16 period_ms);

#endif /* MOTION_MOTION_H */
