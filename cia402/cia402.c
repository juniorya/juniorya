#include "cia402.h"

static cia402_state_t cia402_decode_state(uint16_t statusword)
{
    uint16_t masked = statusword & 0x006FU;
    if ((statusword & 0x004FU) == 0x0000U) {
        return CIA402_STATE_NOT_READY;
    }
    if ((masked & 0x004FU) == 0x0040U) {
        return CIA402_STATE_SWITCH_ON_DISABLED;
    }
    if ((masked & 0x006FU) == 0x0021U) {
        return CIA402_STATE_READY_TO_SWITCH_ON;
    }
    if ((masked & 0x006FU) == 0x0023U) {
        return CIA402_STATE_SWITCHED_ON;
    }
    if ((masked & 0x006FU) == 0x0027U) {
        return CIA402_STATE_OPERATION_ENABLED;
    }
    if ((masked & 0x006FU) == 0x002BU) {
        return CIA402_STATE_QUICK_STOP_ACTIVE;
    }
    if ((statusword & 0x004FU) == 0x000FU) {
        return CIA402_STATE_FAULT;
    }
    if ((statusword & 0x004FU) == 0x0007U) {
        return CIA402_STATE_FAULT_REACTION;
    }
    return CIA402_STATE_SWITCH_ON_DISABLED;
}

void cia402_axis_init(cia402_axis_t *axis, cia402_mode_t mode)
{
    axis->state = CIA402_STATE_NOT_READY;
    axis->mode = mode;
    axis->controlword = 0U;
    axis->target_position = 0;
    axis->target_velocity = 0;
    axis->target_torque = 0;
    axis->halt = false;
    axis->quick_stop = false;
    axis->fault_reset_request = false;
}

void cia402_axis_update(cia402_axis_t *axis, const ethcat_txpdo_t *feedback)
{
    axis->state = cia402_decode_state(feedback->statusword);
    if (axis->state == CIA402_STATE_FAULT && axis->fault_reset_request) {
        axis->fault_reset_request = false;
    }
}

void cia402_axis_command(cia402_axis_t *axis, const q16_16_t *targets, cia402_mode_t mode)
{
    axis->mode = mode;
    axis->target_position = targets[0];
    axis->target_velocity = targets[1];
    axis->target_torque = targets[2];
}

void cia402_axis_build_rxpdo(const cia402_axis_t *axis, ethcat_rxpdo_t *rxpdo)
{
    uint16_t cw = 0U;
    cw |= 0x0006U; /* enable voltage + quick stop */
    if (!axis->quick_stop) {
        cw |= 0x0001U; /* switch on */
    }
    if (!axis->halt && !axis->quick_stop) {
        cw |= 0x0008U; /* enable operation */
    }
    if (axis->halt) {
        cw |= 0x0100U;
    }
    if (axis->fault_reset_request) {
        cw |= 0x0080U;
    }
    rxpdo->controlword = cw;
    rxpdo->mode_of_operation = (uint8_t)axis->mode;
    rxpdo->target_position = axis->target_position;
    rxpdo->target_velocity = axis->target_velocity;
    rxpdo->target_torque = axis->target_torque;
}

void cia402_axis_fault_reset(cia402_axis_t *axis)
{
    axis->fault_reset_request = true;
}
