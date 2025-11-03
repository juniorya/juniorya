#include "motion/motion.h"

#include "kinematics/delta.h"
#include "utils/q16.h"

void motion_default_config(motion_config *cfg)
{
    if (cfg != NULL)
    {
        cfg->period_ms = q16_from_float(0.125f);
    }
}

bool motion_init(motion_controller *motion,
                 const motion_config *cfg,
                 planner_context *planner,
                 ethcat_master *master,
                 calib_context *calib)
{
    if ((motion == NULL) || (cfg == NULL) || (planner == NULL) || (master == NULL) || (calib == NULL))
    {
        return false;
    }
    motion->config = *cfg;
    motion->planner = planner;
    motion->master = master;
    motion->calib = calib;
    for (int i = 0; i < 3; ++i)
    {
        cia402_axis_init(&motion->axes[i]);
        motion->axes[i].state = CIA402_OPERATION_ENABLED;
    }
    return true;
}

void motion_shutdown(motion_controller *motion)
{
    (void)motion;
}

bool motion_step(motion_controller *motion)
{
    if (motion == NULL)
    {
        return false;
    }
    vec3_q16 cart;
    vec3_q16 vel;
    if (!planner_next_sample(motion->planner, &cart, &vel))
    {
        return false;
    }
    q16_16 joints[3];
    if (!delta_inverse(&motion->calib->config, cart, joints))
    {
        return false;
    }
    for (int i = 0; i < 3; ++i)
    {
        cia402_push_setpoint(&motion->axes[i], joints[i], vel.x);
        motion->axes[i].actual = joints[i];
    }
    return ethcat_master_send_setpoints(motion->master, joints);
}
