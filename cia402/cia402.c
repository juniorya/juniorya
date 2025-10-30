/**
 * @file cia402.c
 * @brief CiA-402 finite-state machine implementation with multi-mode support.
 */

#include "cia402.h"

#include "utils/q16.h"

/**
 * @brief Internal helper to refresh the status word based on axis state.
 */
static void cia402_update_status(cia402_axis *axis)
{
    uint16_t status = 0U;
    if (axis->state == CIA402_STATE_SWITCH_ON_DISABLED)
    {
        status |= (1U << 6); /* Switch on disabled */
    }
    if (axis->state == CIA402_STATE_READY_TO_SWITCH_ON || axis->state == CIA402_STATE_SWITCHED_ON ||
        axis->state == CIA402_STATE_OPERATION_ENABLED)
    {
        status |= (1U << 0); /* Ready to switch on */
        status |= (1U << 4); /* Voltage enabled */
    }
    if (axis->state == CIA402_STATE_SWITCHED_ON || axis->state == CIA402_STATE_OPERATION_ENABLED)
    {
        status |= (1U << 1); /* Switched on */
    }
    if (axis->state == CIA402_STATE_OPERATION_ENABLED)
    {
        status |= (1U << 2); /* Operation enabled */
    }
    if (axis->state == CIA402_STATE_QUICK_STOP)
    {
        status |= (1U << 5); /* Quick stop */
    }
    if (axis->state == CIA402_STATE_FAULT || axis->fault_active)
    {
        status |= (1U << 3); /* Fault */
    }
    if (q16_abs(axis->position_error) < q16_from_float(0.01f))
    {
        status |= (1U << 10); /* Target reached */
    }
    axis->status_word = status;
}

/**
 * @brief Integrate motion depending on the selected operation mode.
 */
static void cia402_integrate_mode(cia402_axis *axis)
{
    const q16_16 smoothing = q16_from_int(4);
    switch (axis->mode)
    {
        case CIA402_MODE_PP:
        case CIA402_MODE_IP:
        case CIA402_MODE_CSP:
        {
            q16_16 error = q16_sub(axis->command_position, axis->actual_position);
            q16_16 step = q16_div(error, smoothing, NULL);
            axis->actual_velocity = q16_add(step, axis->feedforward);
            axis->actual_position = q16_add(axis->actual_position, step);
            axis->position_error = error;
            axis->velocity_error = q16_sub(axis->command_velocity, axis->actual_velocity);
            axis->torque_error = q16_sub(axis->command_torque, axis->actual_torque);
            break;
        }
        case CIA402_MODE_PV:
        case CIA402_MODE_CSV:
        {
            q16_16 vel_err = q16_sub(axis->command_velocity, axis->actual_velocity);
            q16_16 vel_step = q16_div(vel_err, smoothing, NULL);
            axis->actual_velocity = q16_add(axis->actual_velocity, vel_step);
            axis->actual_position = q16_add(axis->actual_position, axis->actual_velocity);
            axis->velocity_error = vel_err;
            axis->position_error = q16_sub(axis->command_position, axis->actual_position);
            axis->torque_error = q16_sub(axis->command_torque, axis->actual_torque);
            break;
        }
        case CIA402_MODE_PT:
        case CIA402_MODE_CST:
        {
            q16_16 torque_err = q16_sub(axis->command_torque, axis->actual_torque);
            q16_16 torque_step = q16_div(torque_err, smoothing, NULL);
            axis->actual_torque = q16_add(axis->actual_torque, torque_step);
            axis->actual_velocity = q16_add(axis->actual_velocity, axis->actual_torque);
            axis->actual_position = q16_add(axis->actual_position, axis->actual_velocity);
            axis->torque_error = torque_err;
            axis->velocity_error = q16_sub(axis->command_velocity, axis->actual_velocity);
            axis->position_error = q16_sub(axis->command_position, axis->actual_position);
            break;
        }
        case CIA402_MODE_HM:
        {
            q16_16 error = q16_sub(0, axis->actual_position);
            q16_16 step = q16_div(error, smoothing, NULL);
            axis->actual_velocity = step;
            axis->actual_position = q16_add(axis->actual_position, step);
            axis->position_error = error;
            axis->velocity_error = q16_sub(axis->command_velocity, axis->actual_velocity);
            axis->torque_error = q16_sub(axis->command_torque, axis->actual_torque);
            if (q16_abs(axis->actual_position) < q16_from_float(0.01f))
            {
                axis->command_position = 0;
            }
            break;
        }
        case CIA402_MODE_NONE:
        default:
        {
            axis->position_error = q16_sub(axis->command_position, axis->actual_position);
            axis->velocity_error = q16_sub(axis->command_velocity, axis->actual_velocity);
            axis->torque_error = q16_sub(axis->command_torque, axis->actual_torque);
            break;
        }
    }
}

void cia402_axis_init(cia402_axis *axis, size_t axis_id)
{
    axis->axis_id = axis_id;
    axis->state = CIA402_STATE_SWITCH_ON_DISABLED;
    axis->mode = CIA402_MODE_CSP;
    axis->command_position = 0;
    axis->command_velocity = 0;
    axis->command_torque = 0;
    axis->feedforward = 0;
    axis->actual_position = 0;
    axis->actual_velocity = 0;
    axis->actual_torque = 0;
    axis->position_error = 0;
    axis->velocity_error = 0;
    axis->torque_error = 0;
    axis->status_word = 0U;
    axis->control_word = 0U;
    axis->power_command = false;
    axis->quick_stop_command = false;
    axis->halt_command = false;
    axis->reset_request = false;
    axis->fault_active = false;
}

void cia402_axis_power(cia402_axis *axis, bool enable)
{
    axis->power_command = enable;
    if (enable)
    {
        axis->control_word |= (1U << 0) | (1U << 1) | (1U << 2) | (1U << 3);
    }
    else
    {
        axis->control_word &= (uint16_t)~((1U << 0) | (1U << 1) | (1U << 3));
    }
}

void cia402_axis_quick_stop(cia402_axis *axis)
{
    axis->quick_stop_command = true;
    axis->control_word &= (uint16_t)~(1U << 2);
}

void cia402_axis_halt(cia402_axis *axis, bool enable)
{
    axis->halt_command = enable;
    if (enable)
    {
        axis->control_word |= (1U << 8);
    }
    else
    {
        axis->control_word &= (uint16_t)~(1U << 8);
    }
}

void cia402_axis_release_halt(cia402_axis *axis)
{
    axis->quick_stop_command = false;
    cia402_axis_halt(axis, false);
    axis->control_word |= (1U << 2);
}

void cia402_axis_reset_fault(cia402_axis *axis)
{
    axis->reset_request = true;
    axis->control_word |= (1U << 7);
}

void cia402_axis_set_mode(cia402_axis *axis, cia402_mode mode)
{
    axis->mode = mode;
}

void cia402_push_setpoints(cia402_axis *axis, q16_16 target_pos, q16_16 feedforward)
{
    axis->command_position = target_pos;
    axis->feedforward = feedforward;
}

void cia402_push_velocity(cia402_axis *axis, q16_16 target_vel)
{
    axis->command_velocity = target_vel;
}

void cia402_push_torque(cia402_axis *axis, q16_16 target_torque)
{
    axis->command_torque = target_torque;
}

void cia402_step(cia402_axis *axis)
{
    if (axis->fault_active)
    {
        axis->state = CIA402_STATE_FAULT;
    }

    switch (axis->state)
    {
        case CIA402_STATE_SWITCH_ON_DISABLED:
        {
            axis->actual_velocity = 0;
            axis->actual_torque = 0;
            if (axis->power_command && !axis->fault_active)
            {
                axis->state = CIA402_STATE_READY_TO_SWITCH_ON;
            }
            break;
        }
        case CIA402_STATE_READY_TO_SWITCH_ON:
        {
            if (!axis->power_command)
            {
                axis->state = CIA402_STATE_SWITCH_ON_DISABLED;
            }
            else
            {
                axis->state = CIA402_STATE_SWITCHED_ON;
            }
            break;
        }
        case CIA402_STATE_SWITCHED_ON:
        {
            if (!axis->power_command)
            {
                axis->state = CIA402_STATE_SWITCH_ON_DISABLED;
            }
            else if (axis->quick_stop_command)
            {
                axis->state = CIA402_STATE_QUICK_STOP;
            }
            else
            {
                axis->state = CIA402_STATE_OPERATION_ENABLED;
            }
            break;
        }
        case CIA402_STATE_OPERATION_ENABLED:
        {
            if (!axis->power_command)
            {
                axis->state = CIA402_STATE_SWITCH_ON_DISABLED;
            }
            else if (axis->quick_stop_command || axis->halt_command)
            {
                axis->state = CIA402_STATE_QUICK_STOP;
            }
            else if (axis->fault_active)
            {
                axis->state = CIA402_STATE_FAULT;
            }
            else
            {
                cia402_integrate_mode(axis);
            }
            break;
        }
        case CIA402_STATE_QUICK_STOP:
        {
            axis->actual_velocity = 0;
            axis->actual_torque = 0;
            axis->actual_position = axis->command_position;
            axis->position_error = 0;
            axis->velocity_error = 0;
            axis->torque_error = 0;
            if (!axis->quick_stop_command && axis->power_command && !axis->fault_active)
            {
                axis->state = CIA402_STATE_OPERATION_ENABLED;
            }
            break;
        }
        case CIA402_STATE_FAULT:
        default:
        {
            axis->actual_velocity = 0;
            axis->actual_torque = 0;
            if (axis->reset_request)
            {
                axis->fault_active = false;
                axis->reset_request = false;
                axis->state = CIA402_STATE_SWITCH_ON_DISABLED;
            }
            break;
        }
    }

    if (axis->reset_request)
    {
        axis->reset_request = false;
    }

    cia402_update_status(axis);
}
