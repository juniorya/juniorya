#ifndef CIA402_CIA402_H
#define CIA402_CIA402_H

#include <stdint.h>
#include <stdbool.h>
#include "utils/fixed.h"
#include "ethcat/master.h"

typedef enum {
    CIA402_MODE_CSP = 8,
    CIA402_MODE_CSV = 9,
    CIA402_MODE_CST = 10
} cia402_mode_t;

typedef enum {
    CIA402_STATE_NOT_READY = 0,
    CIA402_STATE_SWITCH_ON_DISABLED,
    CIA402_STATE_READY_TO_SWITCH_ON,
    CIA402_STATE_SWITCHED_ON,
    CIA402_STATE_OPERATION_ENABLED,
    CIA402_STATE_QUICK_STOP_ACTIVE,
    CIA402_STATE_FAULT_REACTION,
    CIA402_STATE_FAULT
} cia402_state_t;

typedef struct {
    cia402_state_t state;
    cia402_mode_t mode;
    uint16_t controlword;
    q16_16_t target_position;
    q16_16_t target_velocity;
    q16_16_t target_torque;
    bool halt;
    bool quick_stop;
    bool fault_reset_request;
} cia402_axis_t;

void cia402_axis_init(cia402_axis_t *axis, cia402_mode_t mode);
void cia402_axis_update(cia402_axis_t *axis, const ethcat_txpdo_t *feedback);
void cia402_axis_command(cia402_axis_t *axis, const q16_16_t *targets, cia402_mode_t mode);
void cia402_axis_build_rxpdo(const cia402_axis_t *axis, ethcat_rxpdo_t *rxpdo);
void cia402_axis_fault_reset(cia402_axis_t *axis);

#endif
