/**
 * @file cnc.h
 * @brief High-level CNC finite state machine controlling motion execution.
 *
 * The CNC core orchestrates trajectory planning, EtherCAT drive interaction, and
 * safety monitoring. It exposes a deterministic state machine that is evaluated
 * from the 1 kHz Sync0 callback provided by the EtherCAT master. The module is
 * intentionally lightweight and keeps heap allocations out of the real-time
 * execution path.
 */
#ifndef CORE_CNC_H
#define CORE_CNC_H

#include <stdbool.h>
#include <stddef.h>

#include "utils/q16.h"
#include "utils/vec3.h"

/**
 * @brief CNC controller states.
 */
typedef enum
{
    CNC_STATE_IDLE = 0, /**< Ready but not yet homed. */
    CNC_STATE_HOMING,   /**< Homing procedure is active. */
    CNC_STATE_READY,    /**< Ready for motion commands. */
    CNC_STATE_RUNNING,  /**< Trajectory is being executed. */
    CNC_STATE_HOLD,     /**< Motion is paused with drives enabled. */
    CNC_STATE_FAULT     /**< Fault detected and motion inhibited. */
} cnc_state_t;

/**
 * @brief Command identifiers for CNC FSM transitions.
 */
typedef enum
{
    CNC_CMD_NONE = 0, /**< No command pending. */
    CNC_CMD_HOME,     /**< Start homing sequence. */
    CNC_CMD_START,    /**< Start executing buffered trajectory. */
    CNC_CMD_HOLD,     /**< Pause motion execution. */
    CNC_CMD_RESUME,   /**< Resume from hold. */
    CNC_CMD_RESET     /**< Clear faults and return to idle. */
} cnc_command_t;

/**
 * @brief Telemetry snapshot captured each control cycle.
 */
typedef struct
{
    cnc_state_t state;      /**< Current state of the controller. */
    vec3_q16 target_xyz;    /**< Last commanded Cartesian position. */
    vec3_q16 actual_xyz;    /**< Cartesian position estimated from feedback. */
    q16_16 feed_override;   /**< Feed override factor in Q16.16. */
    bool fault_active;      /**< true if a fault condition is latched. */
} cnc_telemetry_t;

/**
 * @brief Controller context shared across the runtime.
 */
typedef struct
{
    cnc_state_t state;          /**< Current state. */
    cnc_command_t pending_cmd;  /**< Pending command to be processed. */
    q16_16 feed_override;       /**< Feed override factor. */
    bool drives_enabled;        /**< true if drives are enabled. */
    bool fault_latched;         /**< true if a fault was detected. */
} cnc_context_t;

/**
 * @brief Initialize the CNC controller context.
 *
 * @param ctx Controller context to initialize.
 */
void cnc_init(cnc_context_t *ctx);

/**
 * @brief Submit a command to the controller.
 *
 * Thread-safe wrapper intended for console or OPC UA interactions.
 *
 * @param ctx Controller context.
 * @param cmd Command to enqueue.
 */
void cnc_submit_command(cnc_context_t *ctx, cnc_command_t cmd);

/**
 * @brief Execute one state machine tick from the 1 kHz control loop.
 *
 * @param ctx Controller context.
 * @param feedback Current actual Cartesian position.
 * @param next_target Next target point produced by the planner.
 * @param telemetry Optional pointer that receives telemetry snapshot.
 */
void cnc_tick(cnc_context_t *ctx,
              const vec3_q16 *feedback,
              const vec3_q16 *next_target,
              cnc_telemetry_t *telemetry);

/**
 * @brief Retrieve whether the controller can accept new motion segments.
 *
 * @param ctx Controller context.
 * @return true if motion buffer has capacity.
 */
bool cnc_ready_for_motion(const cnc_context_t *ctx);

#endif /* CORE_CNC_H */
