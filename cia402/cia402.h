/**
 * @file cia402.h
 * @brief CiA-402 state machine helper functions.
 */
#ifndef CIA402_CIA402_H
#define CIA402_CIA402_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "utils/q16.h"

/**
 * @brief CiA-402 drive states.
 */
typedef enum
{
    CIA402_STATE_SWITCH_ON_DISABLED = 0,
    CIA402_STATE_READY_TO_SWITCH_ON,
    CIA402_STATE_SWITCHED_ON,
    CIA402_STATE_OPERATION_ENABLED,
    CIA402_STATE_QUICK_STOP_ACTIVE,
    CIA402_STATE_FAULT
} cia402_state_t;

/**
 * @brief CiA-402 operation modes.
 */
typedef enum
{
    CIA402_MODE_CSP = 8,
    CIA402_MODE_CST = 9,
    CIA402_MODE_CSV = 10
} cia402_mode_t;

/**
 * @brief CiA-402 axis context.
 */
typedef struct
{
    cia402_state_t state;      /**< Current state. */
    cia402_mode_t mode;        /**< Current operation mode. */
    q16_16 target_position;    /**< Target position. */
    q16_16 feedforward;        /**< Feedforward term. */
} cia402_axis_t;

/**
 * @brief Initialize a CiA-402 axis context.
 *
 * @param axis Axis context.
 * @param mode Initial operation mode.
 */
void cia402_init_axis(cia402_axis_t *axis, cia402_mode_t mode);

/**
 * @brief Push setpoints for the given axis.
 *
 * @param axis Axis context.
 * @param target_pos Target position.
 * @param feedforward Feedforward term.
 */
void cia402_push_setpoints(cia402_axis_t *axis,
                           q16_16 target_pos,
                           q16_16 feedforward);

/**
 * @brief Handle a fault transition by resetting state.
 *
 * @param axis Axis context.
 */
void cia402_handle_fault(cia402_axis_t *axis);

#endif /* CIA402_CIA402_H */
