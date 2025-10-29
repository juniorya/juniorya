/**
 * @file motion.c
 * @brief Motion controller implementation.
 */

#include "motion.h"

#include "log.h"

void motion_default_config(motion_config *cfg)
{
    cfg->period_ms = q16_from_int(1);
}

bool motion_init(motion_controller *motion,
                 const motion_config *cfg,
                 planner_context *planner,
                 ethcat_master *master)
{
    motion->config = *cfg;
    motion->planner = planner;
    motion->master = master;
    motion->sample_index = 0U;
    motion->trajectory.count = 0U;
    return true;
}

void motion_step(motion_controller *motion)
{
    if (motion->trajectory.count == 0U)
    {
        if (!spl_time_scale(&motion->planner->active_spline,
                             motion->config.period_ms,
                             &motion->trajectory))
        {
            return;
        }
        motion->sample_index = 0U;
    }
    if (motion->sample_index < motion->trajectory.count)
    {
        vec3_q16 pos = motion->trajectory.position[motion->sample_index];
        ethcat_master_set_target(motion->master, 0, pos.x);
        ethcat_master_set_target(motion->master, 1, pos.y);
        ethcat_master_set_target(motion->master, 2, pos.z);
        motion->sample_index++;
    }
}

void motion_hold(motion_controller *motion)
{
    motion->trajectory.count = 0U;
    motion->sample_index = 0U;
}

void motion_shutdown(motion_controller *motion)
{
    (void)motion;
    log_printf(LOG_INFO, "Motion controller shutdown");
}
