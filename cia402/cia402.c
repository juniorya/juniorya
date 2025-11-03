#include "cia402/cia402.h"

#include <stddef.h>

void cia402_axis_init(cia402_axis *axis)
{
    if (axis == NULL)
    {
        return;
    }
    axis->state = CIA402_SWITCH_ON_DISABLED;
    axis->target = 0;
    axis->actual = 0;
    axis->quick_stop = false;
}

void cia402_set_fault(cia402_axis *axis, bool fault)
{
    if (axis == NULL)
    {
        return;
    }
    axis->state = fault ? CIA402_FAULT : CIA402_SWITCH_ON_DISABLED;
}

void cia402_set_quick_stop(cia402_axis *axis, bool enable)
{
    if (axis != NULL)
    {
        axis->quick_stop = enable;
    }
}

void cia402_enable_operation(cia402_axis *axis)
{
    if (axis == NULL)
    {
        return;
    }
    if (axis->state == CIA402_SWITCHED_ON)
    {
        axis->state = CIA402_OPERATION_ENABLED;
    }
    else if (axis->state == CIA402_READY_TO_SWITCH_ON)
    {
        axis->state = CIA402_SWITCHED_ON;
    }
}

bool cia402_push_setpoint(cia402_axis *axis, q16_16 target_pos, q16_16 feedforward)
{
    (void)feedforward;
    if ((axis == NULL) || (axis->state != CIA402_OPERATION_ENABLED) || axis->quick_stop)
    {
        return false;
    }
    axis->target = target_pos;
    return true;
}

void cia402_tick(cia402_axis *axis)
{
    if (axis == NULL)
    {
        return;
    }
    switch (axis->state)
    {
        case CIA402_SWITCH_ON_DISABLED:
            axis->state = CIA402_READY_TO_SWITCH_ON;
            break;
        case CIA402_READY_TO_SWITCH_ON:
            axis->state = CIA402_SWITCHED_ON;
            break;
        case CIA402_OPERATION_ENABLED:
            axis->actual = axis->target;
            break;
        default:
            break;
    }
}
