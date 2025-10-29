/**
 * @file motion.c
 * @brief Motion execution implementation.
 */
#include "motion.h"

#include <stddef.h>

void motion_init(motion_controller_t *motion,
                 planner_t *planner,
                 ethcat_master_t *master)
{
    motion->planner = planner;
    motion->master = master;
    for (size_t i = 0U; i < 3U; ++i)
    {
        cia402_init_axis(&motion->axes[i], CIA402_MODE_CSP);
    }
}

void motion_sync0_tick(motion_controller_t *motion, q16_16 period_ms)
{
    vec3_q16 target = {0};
    if (planner_step(motion->planner, period_ms, &target))
    {
        for (size_t i = 0U; i < 3U; ++i)
        {
            cia402_push_setpoints(&motion->axes[i], target.z, 0);
            ethcat_axis_setpoint_t setpoint = {target.z, 0, 0x000F};
            ethcat_master_set_axis(motion->master, i, &setpoint);
        }
        ethcat_master_process(motion->master, &target);
    }
}
