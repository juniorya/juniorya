/**
 * @file planner.h
 * @brief Motion planner orchestrating look-ahead and trajectory sampling.
 */

#ifndef PLANNER_H
#define PLANNER_H

#include <stdbool.h>
#include <stddef.h>

#include "planner/lookahead/lookahead.h"
#include "planner/s_curve/s_curve.h"
#include "planner/splines/splines.h"
#include "ml/tinyml_optimizer.h"
#include "utils/q16.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Planner configuration parameters. */
typedef struct
{
    q16_16 max_velocity;      /**< Maximum feed rate in workspace units/s. */
    q16_16 max_acceleration;  /**< Maximum acceleration. */
    q16_16 max_jerk;          /**< Maximum jerk. */
    size_t queue_length;      /**< Maximum number of segments in queue. */
} planner_config;

/** @brief Planner runtime context. */
typedef struct
{
    planner_config config;         /**< Static configuration. */
    lookahead_buffer lookahead;    /**< Look-ahead buffer for multi-segment blending. */
    s_curve_profile current_motion;/**< Current S-curve profile. */
    spl_plan_t active_spline;      /**< Active spline segment. */
    bool spline_valid;             /**< Whether a spline is active. */
    tinyml_network optimizer;      /**< Embedded neural optimiser. */
    bool optimizer_ready;          /**< True when optimiser weights initialised. */
    q16_16 optimizer_last_scale;   /**< Last velocity multiplier suggested by ML. */
} planner_context;

/**
 * @brief Fill configuration structure with defaults.
 * @param cfg Planner configuration to populate.
 */
void planner_default_config(planner_config *cfg);

/**
 * @brief Initialise planner runtime.
 * @param planner Planner instance.
 * @param cfg Configuration parameters.
 * @return True on success.
 */
bool planner_init(planner_context *planner, const planner_config *cfg);

/**
 * @brief Queue a spline plan for execution.
 * @param planner Planner instance.
 * @param plan Spline plan to enqueue.
 * @return True when accepted.
 */
bool planner_enqueue_spline(planner_context *planner, const spl_plan_t *plan);

/**
 * @brief Advance planner state machine; called each control period.
 * @param planner Planner instance.
 */
void planner_step(planner_context *planner);

/**
 * @brief Hold planner output.
 * @param planner Planner instance.
 */
void planner_hold(planner_context *planner);

/**
 * @brief Shutdown planner and release resources.
 * @param planner Planner instance.
 */
void planner_shutdown(planner_context *planner);

#ifdef __cplusplus
}
#endif

#endif
