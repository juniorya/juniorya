/**
 * @file fb.h
 * @brief PLCopen-style motion function blocks bound to CiA-402 axes.
 */

#ifndef CIA402_FB_H
#define CIA402_FB_H

#include <stdbool.h>

#include "cia402.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief MC_Power function block context.
 */
typedef struct
{
    bool enable;        /**< Enable request from application. */
    bool status;        /**< True when drive is operation enabled. */
    bool busy;          /**< True while the state machine is ramping up. */
    bool error;         /**< True when a fault is detected. */
    bool done;          /**< Sticky done flag. */
} cia402_mc_power;

/**
 * @brief Initialise MC_Power function block state.
 *
 * @param fb Function block instance.
 */
void cia402_mc_power_init(cia402_mc_power *fb);

/**
 * @brief Execute MC_Power cycle.
 *
 * @param fb Function block instance.
 * @param axis Target axis.
 */
void cia402_mc_power_cycle(cia402_mc_power *fb, cia402_axis *axis);

/**
 * @brief MC_Reset function block context.
 */
typedef struct
{
    bool execute;   /**< Execute request. */
    bool busy;      /**< Busy flag while reset in progress. */
    bool done;      /**< Indicates the fault reset completed. */
    bool error;     /**< True if reset cannot be performed. */
} cia402_mc_reset;

/**
 * @brief Initialise MC_Reset function block.
 *
 * @param fb Function block instance.
 */
void cia402_mc_reset_init(cia402_mc_reset *fb);

/**
 * @brief Execute MC_Reset cycle.
 *
 * @param fb Function block instance.
 * @param axis Target axis.
 */
void cia402_mc_reset_cycle(cia402_mc_reset *fb, cia402_axis *axis);

/**
 * @brief MC_MoveAbsolute function block context.
 */
typedef struct
{
    bool execute;             /**< Execute request. */
    bool busy;                /**< Busy while motion in progress. */
    bool done;                /**< Done when position reached. */
    bool error;               /**< Error if command rejected. */
    q16_16 position;          /**< Target absolute position. */
    q16_16 velocity;          /**< Requested velocity. */
    q16_16 acceleration;      /**< Requested acceleration (for diagnostics). */
    q16_16 deceleration;      /**< Requested deceleration (for diagnostics). */
    q16_16 position_tolerance;/**< Acceptable position error band. */
    bool command_issued;      /**< Internal flag to prevent re-trigger. */
    q16_16 goal_position;     /**< Internal cached goal. */
} cia402_mc_move_absolute;

/**
 * @brief Initialise MC_MoveAbsolute.
 *
 * @param fb Function block instance.
 */
void cia402_mc_move_absolute_init(cia402_mc_move_absolute *fb);

/**
 * @brief Execute MC_MoveAbsolute cycle.
 *
 * @param fb Function block instance.
 * @param axis Target axis.
 */
void cia402_mc_move_absolute_cycle(cia402_mc_move_absolute *fb, cia402_axis *axis);

/**
 * @brief MC_MoveRelative function block context.
 */
typedef struct
{
    bool execute;             /**< Execute request. */
    bool busy;                /**< Busy while motion in progress. */
    bool done;                /**< Done when relative movement completed. */
    bool error;               /**< Error if command rejected. */
    q16_16 distance;          /**< Relative distance. */
    q16_16 velocity;          /**< Requested velocity. */
    q16_16 acceleration;      /**< Requested acceleration. */
    q16_16 deceleration;      /**< Requested deceleration. */
    q16_16 position_tolerance;/**< Acceptable residual error. */
    bool command_issued;      /**< Internal latch. */
    q16_16 goal_position;     /**< Internal goal position. */
} cia402_mc_move_relative;

/**
 * @brief Initialise MC_MoveRelative block.
 *
 * @param fb Function block instance.
 */
void cia402_mc_move_relative_init(cia402_mc_move_relative *fb);

/**
 * @brief Execute MC_MoveRelative cycle.
 *
 * @param fb Function block instance.
 * @param axis Target axis.
 */
void cia402_mc_move_relative_cycle(cia402_mc_move_relative *fb, cia402_axis *axis);

/**
 * @brief MC_MoveVelocity function block context.
 */
typedef struct
{
    bool execute;    /**< Execute request. */
    bool busy;       /**< Busy while velocity demand active. */
    bool done;       /**< Done when command released. */
    bool error;      /**< Error indicator. */
    q16_16 velocity; /**< Velocity demand. */
} cia402_mc_move_velocity;

/**
 * @brief Initialise MC_MoveVelocity block.
 *
 * @param fb Function block instance.
 */
void cia402_mc_move_velocity_init(cia402_mc_move_velocity *fb);

/**
 * @brief Execute MC_MoveVelocity cycle.
 *
 * @param fb Function block instance.
 * @param axis Target axis.
 */
void cia402_mc_move_velocity_cycle(cia402_mc_move_velocity *fb, cia402_axis *axis);

/**
 * @brief MC_Stop function block context.
 */
typedef struct
{
    bool execute;    /**< Execute request. */
    bool busy;       /**< Busy while decelerating. */
    bool done;       /**< True when stop completed. */
    bool error;      /**< Error indicator. */
    q16_16 decel;    /**< Requested deceleration magnitude. */
} cia402_mc_stop;

/**
 * @brief Initialise MC_Stop block.
 *
 * @param fb Function block instance.
 */
void cia402_mc_stop_init(cia402_mc_stop *fb);

/**
 * @brief Execute MC_Stop cycle.
 *
 * @param fb Function block instance.
 * @param axis Target axis.
 */
void cia402_mc_stop_cycle(cia402_mc_stop *fb, cia402_axis *axis);

/**
 * @brief MC_Halt function block context.
 */
typedef struct
{
    bool execute;    /**< Execute request. */
    bool busy;       /**< Busy while halting. */
    bool done;       /**< Done when halted. */
    bool error;      /**< Error indicator. */
} cia402_mc_halt;

/**
 * @brief Initialise MC_Halt block.
 *
 * @param fb Function block instance.
 */
void cia402_mc_halt_init(cia402_mc_halt *fb);

/**
 * @brief Execute MC_Halt cycle.
 *
 * @param fb Function block instance.
 * @param axis Target axis.
 */
void cia402_mc_halt_cycle(cia402_mc_halt *fb, cia402_axis *axis);

/**
 * @brief MC_TorqueControl function block context.
 */
typedef struct
{
    bool execute;     /**< Execute request. */
    bool busy;        /**< Busy while torque ramp active. */
    bool done;        /**< Done when the torque command is maintained. */
    bool error;       /**< Error indicator. */
    q16_16 torque;    /**< Torque demand. */
    q16_16 velocity;  /**< Optional velocity bias. */
} cia402_mc_torque_control;

/**
 * @brief Initialise MC_TorqueControl block.
 *
 * @param fb Function block instance.
 */
void cia402_mc_torque_control_init(cia402_mc_torque_control *fb);

/**
 * @brief Execute MC_TorqueControl cycle.
 *
 * @param fb Function block instance.
 * @param axis Target axis.
 */
void cia402_mc_torque_control_cycle(cia402_mc_torque_control *fb, cia402_axis *axis);

#ifdef __cplusplus
}
#endif

#endif
