/**
 * @file cia402.c
 * @brief Implementation of CiA-402 helper functions.
 */
#include "cia402.h"

void cia402_init_axis(cia402_axis_t *axis, cia402_mode_t mode)
{
    axis->state = CIA402_STATE_SWITCH_ON_DISABLED;
    axis->mode = mode;
    axis->target_position = 0;
    axis->feedforward = 0;
}

void cia402_push_setpoints(cia402_axis_t *axis,
                           q16_16 target_pos,
                           q16_16 feedforward)
{
    axis->target_position = target_pos;
    axis->feedforward = feedforward;
    axis->state = CIA402_STATE_OPERATION_ENABLED;
}

void cia402_handle_fault(cia402_axis_t *axis)
{
    axis->state = CIA402_STATE_SWITCH_ON_DISABLED;
}
