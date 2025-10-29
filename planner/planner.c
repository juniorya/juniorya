/**
 * @file planner.c
 * @brief Planner coordination logic.
 */

#include "planner.h"
#include "ml/tinyml_optimizer.h"
#include "utils/vec3.h"

#include <string.h>

void planner_default_config(planner_config *cfg)
{
    cfg->max_velocity = q16_from_int(200);
    cfg->max_acceleration = q16_from_int(1000);
    cfg->max_jerk = q16_from_int(5000);
    cfg->queue_length = 32U;
}

static tinyml_segment_feature feature_from_plan(const spl_plan_t *plan, const planner_config *cfg)
{
    tinyml_segment_feature feat;
    feat.arc_length = plan->length;
    feat.target_velocity = cfg->max_velocity;
    feat.target_accel = cfg->max_acceleration;

    vec3_q16 p0;
    vec3_q16 p1;
    vec3_q16 p2;
    (void)spl_sample_arc(plan, q16_from_float(0.25f), &p0, NULL);
    (void)spl_sample_arc(plan, q16_from_float(0.5f), &p1, NULL);
    (void)spl_sample_arc(plan, q16_from_float(0.75f), &p2, NULL);
    vec3_q16 ab = vec3_sub(p1, p0);
    vec3_q16 bc = vec3_sub(p2, p1);
    vec3_q16 cross = vec3_cross(ab, bc);
    q16_16 num = vec3_norm(cross);
    q16_16 denom = q16_mul(vec3_norm(ab), vec3_norm(bc));
    bool ok = true;
    if ((denom == 0) || (num == 0))
    {
        feat.curvature = q16_from_int(0);
    }
    else
    {
        feat.curvature = q16_div(num, denom, &ok);
        if (!ok)
        {
            feat.curvature = q16_from_int(0);
        }
    }
    return feat;
}

bool planner_init(planner_context *planner, const planner_config *cfg)
{
    planner->config = *cfg;
    lookahead_init(&planner->lookahead, cfg->queue_length);
    planner->spline_valid = false;
    tinyml_network_init(&planner->optimizer);
    planner->optimizer_ready = true;
    planner->optimizer_last_scale = q16_from_int(1);
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
            if (planner->optimizer_ready)
            {
                tinyml_segment_feature feat = feature_from_plan(&planner->active_spline, &planner->config);
                q16_16 scale;
                if (tinyml_optimize_segments(&planner->optimizer, &feat, 1U, &scale))
                {
                    planner->optimizer_last_scale = scale;
                }
            }
            q16_16 max_vel = q16_mul(planner->config.max_velocity, planner->optimizer_last_scale);
            q16_16 max_acc = q16_mul(planner->config.max_acceleration, planner->optimizer_last_scale);
            q16_16 max_jerk = q16_mul(planner->config.max_jerk, planner->optimizer_last_scale);
            s_curve_generate(&planner->current_motion,
                             max_vel,
                             max_acc,
                             max_jerk,
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
