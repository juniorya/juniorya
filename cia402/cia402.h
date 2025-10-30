/**
 * @file cia402.h
 * @brief CiA-402 state machine, operation mode handling, and PLCopen function block support types.
 */

#ifndef CIA402_H
#define CIA402_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "utils/q16.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief CiA-402 state enumeration mirroring the DS402 state diagram.
 */
typedef enum
{
    CIA402_STATE_SWITCH_ON_DISABLED = 0, /**< Power stage disabled and safe. */
    CIA402_STATE_READY_TO_SWITCH_ON,     /**< Device ready to transition to enable. */
    CIA402_STATE_SWITCHED_ON,            /**< Device has power but torque disabled. */
    CIA402_STATE_OPERATION_ENABLED,      /**< Motion commands accepted. */
    CIA402_STATE_QUICK_STOP,             /**< Quick stop active. */
    CIA402_STATE_FAULT                   /**< Fault latched. */
} cia402_state;

/**
 * @brief CiA-402 operation modes supported by the library.
 */
typedef enum
{
    CIA402_MODE_NONE = 0,   /**< Mode not selected. */
    CIA402_MODE_PP,         /**< Profile position mode. */
    CIA402_MODE_PV,         /**< Profile velocity mode. */
    CIA402_MODE_PT,         /**< Profile torque mode. */
    CIA402_MODE_HM,         /**< Homing mode. */
    CIA402_MODE_IP,         /**< Interpolated position mode. */
    CIA402_MODE_CSP,        /**< Cyclic synchronous position. */
    CIA402_MODE_CSV,        /**< Cyclic synchronous velocity. */
    CIA402_MODE_CST         /**< Cyclic synchronous torque. */
} cia402_mode;

/**
 * @brief CiA-402 axis runtime representation.
 */
typedef struct
{
    size_t axis_id;           /**< Logical axis identifier. */
    cia402_state state;       /**< Current state machine position. */
    cia402_mode mode;         /**< Active operation mode. */
    q16_16 command_position;  /**< Requested position target. */
    q16_16 command_velocity;  /**< Requested velocity target. */
    q16_16 command_torque;    /**< Requested torque target. */
    q16_16 feedforward;       /**< Feed-forward component for synchronous modes. */
    q16_16 actual_position;   /**< Simulated actual position. */
    q16_16 actual_velocity;   /**< Simulated actual velocity. */
    q16_16 actual_torque;     /**< Simulated actual torque. */
    q16_16 position_error;    /**< Position tracking error. */
    q16_16 velocity_error;    /**< Velocity tracking error. */
    q16_16 torque_error;      /**< Torque tracking error. */
    uint16_t status_word;     /**< Status word mirror (CiA-402 table 46). */
    uint16_t control_word;    /**< Control word mirror. */
    bool power_command;       /**< Power enable request from higher layer. */
    bool quick_stop_command;  /**< Quick stop request flag. */
    bool halt_command;        /**< Halt request flag. */
    bool reset_request;       /**< Fault reset request flag. */
    bool fault_active;        /**< Latched fault indicator. */
} cia402_axis;

/**
 * @brief Initialise CiA-402 axis data structure.
 *
 * @param axis Axis instance.
 * @param axis_id Logical axis identifier.
 */
void cia402_axis_init(cia402_axis *axis, size_t axis_id);

/**
 * @brief Request power enable or disable.
 *
 * @param axis Axis instance.
 * @param enable True to request power stage enable, false to disable.
 */
void cia402_axis_power(cia402_axis *axis, bool enable);

/**
 * @brief Issue a quick stop request.
 *
 * @param axis Axis instance.
 */
void cia402_axis_quick_stop(cia402_axis *axis);

/**
 * @brief Request a controlled halt without quick stop deceleration.
 *
 * @param axis Axis instance.
 * @param enable True to request halt, false to resume.
 */
void cia402_axis_halt(cia402_axis *axis, bool enable);

/**
 * @brief Clear quick stop or halt requests once handled.
 *
 * @param axis Axis instance.
 */
void cia402_axis_release_halt(cia402_axis *axis);

/**
 * @brief Apply a fault reset request.
 *
 * @param axis Axis instance.
 */
void cia402_axis_reset_fault(cia402_axis *axis);

/**
 * @brief Select the operation mode.
 *
 * @param axis Axis instance.
 * @param mode Requested mode.
 */
void cia402_axis_set_mode(cia402_axis *axis, cia402_mode mode);

/**
 * @brief Push synchronous position command (CSP/IP/PP).
 *
 * @param axis Axis instance.
 * @param target_pos Target position set-point.
 * @param feedforward Optional velocity feed-forward term.
 */
void cia402_push_setpoints(cia402_axis *axis, q16_16 target_pos, q16_16 feedforward);

/**
 * @brief Push velocity target for PV/CSV modes.
 *
 * @param axis Axis instance.
 * @param target_vel Velocity command.
 */
void cia402_push_velocity(cia402_axis *axis, q16_16 target_vel);

/**
 * @brief Push torque target for PT/CST modes.
 *
 * @param axis Axis instance.
 * @param target_torque Torque command.
 */
void cia402_push_torque(cia402_axis *axis, q16_16 target_torque);

/**
 * @brief Advance CiA-402 state machine and internal plant simulation.
 *
 * @param axis Axis instance.
 */
void cia402_step(cia402_axis *axis);

#ifdef __cplusplus
}
#endif

#endif
