#ifndef PLANNER_H
#define PLANNER_H

#include <stdbool.h>
#include <stddef.h>

#include "planner/lookahead/lookahead.h"
#include "planner/s_curve/s_curve.h"
#include "planner/splines/splines.h"

/** \brief Planner configuration. */
typedef struct
{
    q16_16 max_velocity; /**< Maximum axis velocity. */
    q16_16 max_accel;    /**< Maximum axis acceleration. */
    q16_16 max_jerk;     /**< Maximum jerk. */
    q16_16 period_ms;    /**< Sampling period in milliseconds. */
} planner_config;

/** \brief Planner context. */
typedef struct
{
    planner_config config;          /**< Active configuration. */
    vec3_q16 current_position;      /**< Current machine position. */
    time_scaled_traj_t segments[16];/**< Segment queue. */
    size_t head;                    /**< Queue head. */
    size_t tail;                    /**< Queue tail. */
    size_t index;                   /**< Active sample index. */
    bool active;                    /**< Whether a segment is active. */
} planner_context;

void planner_default_config(planner_config *cfg);

bool planner_init(planner_context *ctx, const planner_config *cfg);

void planner_shutdown(planner_context *ctx);

bool planner_queue_cartesian(planner_context *ctx, vec3_q16 target, q16_16 feed);

bool planner_queue_spline(planner_context *ctx, const spl_plan_t *plan);

bool planner_next_sample(planner_context *ctx, vec3_q16 *out_position, vec3_q16 *out_velocity);

#endif
