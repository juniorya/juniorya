/**
 * @file cia402.h
 * @brief CiA-402 state machine and CSP setpoint interface.
 */

#ifndef CIA402_H
#define CIA402_H

#include <stdbool.h>
#include <stddef.h>

#include "utils/q16.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief CiA-402 state enumeration. */
typedef enum
{
    CIA402_STATE_SWITCH_ON_DISABLED = 0,
    CIA402_STATE_READY_TO_SWITCH_ON,
    CIA402_STATE_SWITCHED_ON,
    CIA402_STATE_OPERATION_ENABLED,
    CIA402_STATE_QUICK_STOP,
    CIA402_STATE_FAULT
} cia402_state;

/** @brief Axis instance for CiA-402 control. */
typedef struct
{
    size_t axis_id;       /**< Logical axis identifier. */
    cia402_state state;   /**< Current state. */
    q16_16 target_pos;    /**< Target position. */
    q16_16 actual_pos;    /**< Actual position. */
} cia402_axis;

/**
 * @brief Initialise CiA-402 axis.
 * @param axis Axis instance.
 * @param axis_id Logical axis identifier.
 */
void cia402_axis_init(cia402_axis *axis, size_t axis_id);

/**
 * @brief Push CSP setpoint to axis.
 * @param axis Axis instance.
 * @param target_pos Target position.
 * @param feedforward Optional feedforward term.
 */
void cia402_push_setpoints(cia402_axis *axis, q16_16 target_pos, q16_16 feedforward);

/**
 * @brief Progress CiA-402 state machine.
 * @param axis Axis instance.
 */
void cia402_step(cia402_axis *axis);

#ifdef __cplusplus
}
#endif

#endif
