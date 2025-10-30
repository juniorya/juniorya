/**
 * @file fb.c
 * @brief PLCopen-style motion function blocks mapped to CiA-402 axes.
 */

#include "fb.h"

#include "utils/q16.h"

static bool cia402_axis_ready(const cia402_axis *axis)
{
    return (axis->state == CIA402_STATE_OPERATION_ENABLED) && !axis->fault_active;
}

static q16_16 cia402_default_tolerance(void)
{
    return q16_from_float(0.01f);
}

void cia402_mc_power_init(cia402_mc_power *fb)
{
    fb->enable = false;
    fb->status = false;
    fb->busy = false;
    fb->error = false;
    fb->done = false;
}

void cia402_mc_power_cycle(cia402_mc_power *fb, cia402_axis *axis)
{
    if (!fb->enable)
    {
        cia402_axis_power(axis, false);
        fb->status = false;
        fb->busy = false;
        fb->error = false;
        fb->done = false;
        return;
    }

    cia402_axis_power(axis, true);
    fb->error = axis->fault_active || (axis->state == CIA402_STATE_FAULT);
    fb->busy = (axis->state == CIA402_STATE_SWITCH_ON_DISABLED) ||
               (axis->state == CIA402_STATE_READY_TO_SWITCH_ON) ||
               (axis->state == CIA402_STATE_SWITCHED_ON);
    fb->status = cia402_axis_ready(axis);
    if (fb->status)
    {
        fb->done = true;
    }
    if (fb->error)
    {
        fb->done = false;
    }
}

void cia402_mc_reset_init(cia402_mc_reset *fb)
{
    fb->execute = false;
    fb->busy = false;
    fb->done = false;
    fb->error = false;
}

void cia402_mc_reset_cycle(cia402_mc_reset *fb, cia402_axis *axis)
{
    if (!fb->execute)
    {
        fb->busy = false;
        fb->done = false;
        fb->error = false;
        return;
    }

    cia402_axis_reset_fault(axis);
    fb->busy = (axis->state == CIA402_STATE_FAULT);
    fb->error = false;
    if (!axis->fault_active && axis->state != CIA402_STATE_FAULT)
    {
        fb->busy = false;
        fb->done = true;
    }
}

void cia402_mc_move_absolute_init(cia402_mc_move_absolute *fb)
{
    fb->execute = false;
    fb->busy = false;
    fb->done = false;
    fb->error = false;
    fb->position = 0;
    fb->velocity = 0;
    fb->acceleration = 0;
    fb->deceleration = 0;
    fb->position_tolerance = 0;
    fb->command_issued = false;
    fb->goal_position = 0;
}

void cia402_mc_move_absolute_cycle(cia402_mc_move_absolute *fb, cia402_axis *axis)
{
    q16_16 tolerance = fb->position_tolerance;
    if (tolerance == 0)
    {
        tolerance = cia402_default_tolerance();
    }

    if (!fb->execute)
    {
        fb->busy = false;
        fb->done = false;
        fb->error = false;
        fb->command_issued = false;
        return;
    }

    if (!cia402_axis_ready(axis))
    {
        fb->error = true;
        fb->busy = false;
        fb->done = false;
        return;
    }

    if (!fb->command_issued)
    {
        fb->goal_position = fb->position;
        cia402_axis_set_mode(axis, CIA402_MODE_PP);
        cia402_push_setpoints(axis, fb->goal_position, fb->velocity);
        cia402_push_velocity(axis, fb->velocity);
        fb->command_issued = true;
        fb->busy = true;
        fb->done = false;
    }
    else
    {
        cia402_push_setpoints(axis, fb->goal_position, fb->velocity);
        if (q16_abs(axis->position_error) <= tolerance)
        {
            fb->busy = false;
            fb->done = true;
        }
        else
        {
            fb->busy = true;
        }
    }
}

void cia402_mc_move_relative_init(cia402_mc_move_relative *fb)
{
    fb->execute = false;
    fb->busy = false;
    fb->done = false;
    fb->error = false;
    fb->distance = 0;
    fb->velocity = 0;
    fb->acceleration = 0;
    fb->deceleration = 0;
    fb->position_tolerance = 0;
    fb->command_issued = false;
    fb->goal_position = 0;
}

void cia402_mc_move_relative_cycle(cia402_mc_move_relative *fb, cia402_axis *axis)
{
    q16_16 tolerance = fb->position_tolerance;
    if (tolerance == 0)
    {
        tolerance = cia402_default_tolerance();
    }

    if (!fb->execute)
    {
        fb->busy = false;
        fb->done = false;
        fb->error = false;
        fb->command_issued = false;
        return;
    }

    if (!cia402_axis_ready(axis))
    {
        fb->error = true;
        fb->busy = false;
        fb->done = false;
        return;
    }

    if (!fb->command_issued)
    {
        fb->goal_position = q16_add(axis->actual_position, fb->distance);
        cia402_axis_set_mode(axis, CIA402_MODE_PP);
        cia402_push_setpoints(axis, fb->goal_position, fb->velocity);
        cia402_push_velocity(axis, fb->velocity);
        fb->command_issued = true;
        fb->busy = true;
        fb->done = false;
    }
    else
    {
        cia402_push_setpoints(axis, fb->goal_position, fb->velocity);
        if (q16_abs(axis->position_error) <= tolerance)
        {
            fb->busy = false;
            fb->done = true;
        }
        else
        {
            fb->busy = true;
        }
    }
}

void cia402_mc_move_velocity_init(cia402_mc_move_velocity *fb)
{
    fb->execute = false;
    fb->busy = false;
    fb->done = false;
    fb->error = false;
    fb->velocity = 0;
}

void cia402_mc_move_velocity_cycle(cia402_mc_move_velocity *fb, cia402_axis *axis)
{
    if (!fb->execute)
    {
        cia402_push_velocity(axis, 0);
        fb->busy = false;
        fb->done = true;
        fb->error = false;
        return;
    }

    if (!cia402_axis_ready(axis))
    {
        fb->error = true;
        fb->busy = false;
        fb->done = false;
        return;
    }

    cia402_axis_set_mode(axis, CIA402_MODE_CSV);
    cia402_push_velocity(axis, fb->velocity);
    fb->busy = true;
    fb->done = false;
    fb->error = false;
}

void cia402_mc_stop_init(cia402_mc_stop *fb)
{
    fb->execute = false;
    fb->busy = false;
    fb->done = false;
    fb->error = false;
    fb->decel = 0;
}

void cia402_mc_stop_cycle(cia402_mc_stop *fb, cia402_axis *axis)
{
    q16_16 tolerance = cia402_default_tolerance();
    if (!fb->execute)
    {
        if (fb->done)
        {
            cia402_axis_release_halt(axis);
        }
        fb->busy = false;
        fb->done = false;
        fb->error = false;
        return;
    }

    if (axis->fault_active && axis->state == CIA402_STATE_FAULT)
    {
        fb->error = true;
        fb->busy = false;
        fb->done = false;
        return;
    }

    cia402_axis_quick_stop(axis);
    cia402_axis_halt(axis, true);
    fb->busy = true;
    fb->error = false;
    if (q16_abs(axis->actual_velocity) <= tolerance)
    {
        cia402_axis_release_halt(axis);
        fb->busy = false;
        fb->done = true;
    }
    else
    {
        fb->done = false;
    }
}

void cia402_mc_halt_init(cia402_mc_halt *fb)
{
    fb->execute = false;
    fb->busy = false;
    fb->done = false;
    fb->error = false;
}

void cia402_mc_halt_cycle(cia402_mc_halt *fb, cia402_axis *axis)
{
    q16_16 tolerance = cia402_default_tolerance();
    if (!fb->execute)
    {
        cia402_axis_halt(axis, false);
        fb->busy = false;
        fb->done = false;
        fb->error = false;
        return;
    }

    if (!cia402_axis_ready(axis))
    {
        fb->error = true;
        fb->busy = false;
        fb->done = false;
        return;
    }

    cia402_axis_halt(axis, true);
    cia402_push_velocity(axis, 0);
    fb->busy = q16_abs(axis->actual_velocity) > tolerance;
    fb->done = !fb->busy;
    fb->error = false;
}

void cia402_mc_torque_control_init(cia402_mc_torque_control *fb)
{
    fb->execute = false;
    fb->busy = false;
    fb->done = false;
    fb->error = false;
    fb->torque = 0;
    fb->velocity = 0;
}

void cia402_mc_torque_control_cycle(cia402_mc_torque_control *fb, cia402_axis *axis)
{
    q16_16 tolerance = cia402_default_tolerance();
    if (!fb->execute)
    {
        cia402_push_torque(axis, 0);
        fb->busy = false;
        fb->done = false;
        fb->error = false;
        return;
    }

    if (!cia402_axis_ready(axis))
    {
        fb->error = true;
        fb->busy = false;
        fb->done = false;
        return;
    }

    cia402_axis_set_mode(axis, CIA402_MODE_CST);
    cia402_push_velocity(axis, fb->velocity);
    cia402_push_torque(axis, fb->torque);
    fb->busy = q16_abs(axis->torque_error) > tolerance;
    fb->done = !fb->busy;
    fb->error = false;
}
