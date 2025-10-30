/**
 * @file cia402.c
 * @brief CiA-402 finite-state machine implementation.
 */

#include "cia402.h"

void cia402_axis_init(cia402_axis *axis, size_t axis_id)
{
    axis->axis_id = axis_id;
    axis->state = CIA402_STATE_SWITCH_ON_DISABLED;
    axis->target_pos = 0;
    axis->actual_pos = 0;
}

void cia402_push_setpoints(cia402_axis *axis, q16_16 target_pos, q16_16 feedforward)
{
    (void)feedforward;
    axis->target_pos = target_pos;
}

void cia402_step(cia402_axis *axis)
{
    switch (axis->state)
    {
        case CIA402_STATE_SWITCH_ON_DISABLED:
            axis->state = CIA402_STATE_READY_TO_SWITCH_ON;
            break;
        case CIA402_STATE_READY_TO_SWITCH_ON:
            axis->state = CIA402_STATE_SWITCHED_ON;
            break;
        case CIA402_STATE_SWITCHED_ON:
            axis->state = CIA402_STATE_OPERATION_ENABLED;
            break;
        case CIA402_STATE_OPERATION_ENABLED:
            axis->actual_pos = q16_add(axis->actual_pos,
                                       q16_div(q16_sub(axis->target_pos, axis->actual_pos),
                                               q16_from_int(2), NULL));
            break;
        case CIA402_STATE_QUICK_STOP:
            axis->actual_pos = axis->target_pos;
            break;
        case CIA402_STATE_FAULT:
        default:
            break;
    }
}
