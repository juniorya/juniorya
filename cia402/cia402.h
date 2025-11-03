#ifndef CIA402_H
#define CIA402_H

#include <stdbool.h>
#include <stdint.h>

#include "utils/q16.h"

/** \brief CiA-402 state enumeration. */
typedef enum
{
    CIA402_NOT_READY,
    CIA402_SWITCH_ON_DISABLED,
    CIA402_READY_TO_SWITCH_ON,
    CIA402_SWITCHED_ON,
    CIA402_OPERATION_ENABLED,
    CIA402_FAULT
} cia402_state;

/** \brief Axis descriptor. */
typedef struct
{
    cia402_state state;   /**< Current state. */
    q16_16 target;        /**< Target position. */
    q16_16 actual;        /**< Actual position. */
    bool quick_stop;      /**< Quick stop request. */
} cia402_axis;

void cia402_axis_init(cia402_axis *axis);

void cia402_set_fault(cia402_axis *axis, bool fault);

void cia402_set_quick_stop(cia402_axis *axis, bool enable);

void cia402_enable_operation(cia402_axis *axis);

bool cia402_push_setpoint(cia402_axis *axis, q16_16 target_pos, q16_16 feedforward);

void cia402_tick(cia402_axis *axis);

#endif
