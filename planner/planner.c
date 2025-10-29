/**
 * @file planner.c
 * @brief Planner coordination logic.
 */

#include "planner.h"

#include <string.h>

void planner_default_config(planner_config *cfg)
{
    cfg->max_velocity = q16_from_int(200);
    cfg->max_acceleration = q16_from_int(1000);
    cfg->max_jerk = q16_from_int(5000);
    cfg->queue_length = 32U;
}

bool planner_init(planner_context *planner, const planner_config *cfg)
{
    planner->config = *cfg;
    lookahead_init(&planner->lookahead, cfg->queue_length);
    planner->spline_valid = false;
    return true;
}

bool planner_enqueue_spline(planner_context *planner, const spl_plan_t *plan)
{
    return lookahead_push(&planner->lookahead, plan);
}

void planner_step(planner_context *planner)
{
    if (!planner->spline_valid)
    {
        if (lookahead_pop(&planner->lookahead, &planner->active_spline))
        {
            planner->spline_valid = true;
            s_curve_generate(&planner->current_motion,
                             planner->config.max_velocity,
                             planner->config.max_acceleration,
                             planner->config.max_jerk,
                             &planner->active_spline);
        }
    }
    if (planner->spline_valid)
    {
        s_curve_step(&planner->current_motion);
        if (planner->current_motion.complete)
        {
            planner->spline_valid = false;
        }
    }
}

void planner_hold(planner_context *planner)
{
    planner->spline_valid = false;
    lookahead_clear(&planner->lookahead);
}

void planner_shutdown(planner_context *planner)
{
    lookahead_deinit(&planner->lookahead);
    (void)planner;
}
