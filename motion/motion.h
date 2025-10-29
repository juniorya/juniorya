/**
 * @file motion.h
 * @brief Motion controller bridging planner outputs to EtherCAT setpoints.
 */

#ifndef MOTION_H
#define MOTION_H

#include <stdbool.h>

#include "ethcat/ethcat.h"
#include "planner/planner.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Motion controller configuration. */
typedef struct
{
    q16_16 period_ms; /**< Control period in milliseconds. */
} motion_config;

/** @brief Motion controller context. */
typedef struct
{
    motion_config config;          /**< Static configuration. */
    planner_context *planner;      /**< Planner providing trajectories. */
    ethcat_master *master;         /**< EtherCAT master used for setpoints. */
    size_t sample_index;           /**< Current sample index in active spline. */
    time_scaled_traj_t trajectory; /**< Cached trajectory samples. */
} motion_controller;

/**
 * @brief Fill configuration with defaults.
 * @param cfg Configuration structure.
 */
void motion_default_config(motion_config *cfg);

/**
 * @brief Initialise motion controller.
 * @param motion Controller instance.
 * @param cfg Configuration parameters.
 * @param planner Planner instance.
 * @param master EtherCAT master instance.
 * @return True on success.
 */
bool motion_init(motion_controller *motion,
                 const motion_config *cfg,
                 planner_context *planner,
                 ethcat_master *master);

/**
 * @brief Execute control loop step.
 * @param motion Controller instance.
 */
void motion_step(motion_controller *motion);

/**
 * @brief Hold motion output.
 * @param motion Controller instance.
 */
void motion_hold(motion_controller *motion);

/**
 * @brief Shut down controller.
 * @param motion Controller instance.
 */
void motion_shutdown(motion_controller *motion);

#ifdef __cplusplus
}
#endif

#endif
