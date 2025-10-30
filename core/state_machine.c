/**
 * @file state_machine.c
 * @brief Implementation of CNC finite-state machine.
 */

#include "state_machine.h"

#include "cia402/cia402.h"
#include "ethcat/ethcat.h"
#include "motion/motion.h"
#include "planner/planner.h"

bool state_machine_should_run(const cnc_context *cnc)
{
    return cnc->state != CNC_STATE_FAULT;
}

void state_machine_step(cnc_context *cnc)
{
    switch (cnc->state)
    {
        case CNC_STATE_IDLE:
            break;
        case CNC_STATE_RUNNING:
            planner_step(cnc->planner);
            motion_step(cnc->motion);
            ethcat_master_update(cnc->master);
            break;
        case CNC_STATE_HOLD:
            motion_hold(cnc->motion);
            break;
        case CNC_STATE_FAULT:
        default:
            break;
    }
}
