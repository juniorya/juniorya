/**
 * @file cnc.c
 * @brief CNC supervisor implementation.
 */

#include "cnc.h"
#include "commands.h"
#include "state_machine.h"

#include "log.h"
#include "osal/osal.h"

void cnc_default_config(cnc_config *cfg)
{
    cfg->control_period_us = 1000U;
    cfg->enable_g5 = true;
}

bool cnc_init(cnc_context *cnc,
              const cnc_config *cfg,
              drivers_context *drivers,
              planner_context *planner,
              motion_controller *motion,
              ethcat_master *master)
{
    cnc->config = *cfg;
    cnc->state = CNC_STATE_IDLE;
    cnc->drivers = drivers;
    cnc->planner = planner;
    cnc->motion = motion;
    cnc->master = master;
    gcode_parser_init(&cnc->parser, cfg->enable_g5);
    cia402_axis_init(&cnc->axis[0], 0);
    cia402_axis_init(&cnc->axis[1], 1);
    cia402_axis_init(&cnc->axis[2], 2);
    return true;
}

void cnc_run(cnc_context *cnc)
{
    log_printf(LOG_INFO, "CNC main loop started");
    uint64_t last_tick = osal_time_us();
    while (state_machine_should_run(cnc))
    {
        uint64_t now = osal_time_us();
        if (now - last_tick >= cnc->config.control_period_us)
        {
            last_tick = now;
            state_machine_step(cnc);
        }
        else
        {
            osal_sleep_ms(1U);
        }
    }
}

void cnc_request_hold(cnc_context *cnc)
{
    cnc->state = CNC_STATE_HOLD;
}

void cnc_reset_fault(cnc_context *cnc)
{
    cnc->state = CNC_STATE_IDLE;
}

void cnc_shutdown(cnc_context *cnc)
{
    (void)cnc;
    log_printf(LOG_INFO, "CNC shutdown complete");
}
