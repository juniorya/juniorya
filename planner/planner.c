#include "planner/planner.h"
#include "utils/q16.h"

#include <string.h>

void planner_default_config(planner_config *cfg)
{
    if (cfg == NULL)
    {
        return;
    }
    cfg->max_velocity = q16_from_int(200);
    cfg->max_accel = q16_from_int(1000);
    cfg->max_jerk = q16_from_int(5000);
    cfg->period_ms = q16_from_float(0.125f);
}

bool planner_init(planner_context *ctx, const planner_config *cfg)
{
    if ((ctx == NULL) || (cfg == NULL))
    {
        return false;
    }
    ctx->config = *cfg;
    ctx->current_position.x = 0;
    ctx->current_position.y = 0;
    ctx->current_position.z = 0;
    ctx->head = 0U;
    ctx->tail = 0U;
    ctx->index = 0U;
    ctx->active = false;
    return true;
}

void planner_shutdown(planner_context *ctx)
{
    (void)ctx;
}

static bool planner_queue_segment(planner_context *ctx, const time_scaled_traj_t *traj)
{
    size_t next_tail = (ctx->tail + 1U) % 16U;
    if (next_tail == ctx->head)
    {
        return false;
    }
    ctx->segments[ctx->tail] = *traj;
    ctx->tail = next_tail;
    return true;
}

bool planner_queue_spline(planner_context *ctx, const spl_plan_t *plan)
{
    if ((ctx == NULL) || (plan == NULL))
    {
        return false;
    }
    time_scaled_traj_t traj;
    if (!spl_time_scale(plan, ctx->config.period_ms, &traj))
    {
        return false;
    }
    return planner_queue_segment(ctx, &traj);
}

bool planner_queue_cartesian(planner_context *ctx, vec3_q16 target, q16_16 feed)
{
    (void)feed;
    if (ctx == NULL)
    {
        return false;
    }
    vec3_q16 waypoints[2];
    waypoints[0] = ctx->current_position;
    waypoints[1] = target;
    spl_plan_t plan;
    if (!spl_make_from_waypoints(waypoints, 2U, NULL, NULL, &plan))
    {
        return false;
    }
    plan.max_vel = ctx->config.max_velocity;
    plan.max_acc = ctx->config.max_accel;
    plan.max_jerk = ctx->config.max_jerk;
    return planner_queue_spline(ctx, &plan);
}

static bool planner_activate_next(planner_context *ctx)
{
    if (ctx->head == ctx->tail)
    {
        return false;
    }
    ctx->index = 0U;
    ctx->active = true;
    return true;
}

bool planner_next_sample(planner_context *ctx, vec3_q16 *out_position, vec3_q16 *out_velocity)
{
    if ((ctx == NULL) || (out_position == NULL))
    {
        return false;
    }
    if (!ctx->active)
    {
        if (!planner_activate_next(ctx))
        {
            *out_position = ctx->current_position;
            if (out_velocity != NULL)
            {
                out_velocity->x = 0;
                out_velocity->y = 0;
                out_velocity->z = 0;
            }
            return false;
        }
    }
    time_scaled_traj_t *traj = &ctx->segments[ctx->head];
    if (ctx->index >= traj->count)
    {
        ctx->head = (ctx->head + 1U) % 16U;
        ctx->active = false;
        return planner_next_sample(ctx, out_position, out_velocity);
    }
    *out_position = traj->position[ctx->index];
    ctx->current_position = *out_position;
    if (out_velocity != NULL)
    {
        *out_velocity = traj->velocity[ctx->index];
    }
    ctx->index++;
    return true;
}
