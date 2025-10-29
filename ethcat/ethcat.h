/**
 * @file ethcat.h
 * @brief EtherCAT master abstraction with CiA-402 integration hooks.
 *
 * The EtherCAT master is modeled after SOEM. It provides deterministic Sync0
 * callbacks at 1 kHz and manages PDO/SDO transfers. The implementation included
 * here simulates the real hardware but preserves the API so the same interface
 * can be used on the embedded targets.
 */
#ifndef ETHCAT_ETHCAT_H
#define ETHCAT_ETHCAT_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include "utils/vec3.h"

/**
 * @brief Axis feedback collected from the drives.
 */
typedef struct
{
    q16_16 position; /**< Actual position in encoder units. */
    q16_16 velocity; /**< Actual velocity in encoder units per second. */
    q16_16 following_error; /**< Following error. */
    uint16_t status_word; /**< CiA-402 status word. */
} ethcat_axis_feedback_t;

/**
 * @brief Axis setpoints pushed to the drives.
 */
typedef struct
{
    q16_16 target_position; /**< Target position. */
    q16_16 target_velocity; /**< Feedforward velocity. */
    uint16_t control_word;  /**< CiA-402 control word. */
} ethcat_axis_setpoint_t;

/**
 * @brief EtherCAT master context.
 */
typedef struct
{
    uint32_t cycle_time_ns;                   /**< Sync0 period in nanoseconds. */
    ethcat_axis_feedback_t feedback[3];       /**< Feedback for three axes. */
    ethcat_axis_setpoint_t setpoints[3];      /**< Setpoints for three axes. */
    bool link_up;                             /**< Link status. */
} ethcat_master_t;

/**
 * @brief Initialize the EtherCAT master.
 *
 * @param master Master context.
 * @param cycle_time_ns Desired Sync0 period in nanoseconds.
 * @return true on success.
 */
bool ethcat_master_init(ethcat_master_t *master, uint32_t cycle_time_ns);

/**
 * @brief Process one Sync0 cycle.
 *
 * @param master Master context.
 * @param cartesian Cartesian setpoint for the delta robot.
 */
void ethcat_master_process(ethcat_master_t *master, const vec3_q16 *cartesian);

/**
 * @brief Submit a set of CiA-402 setpoints.
 *
 * @param master Master context.
 * @param axis Axis index (0..2).
 * @param setpoint Setpoint structure.
 */
void ethcat_master_set_axis(ethcat_master_t *master,
                            size_t axis,
                            const ethcat_axis_setpoint_t *setpoint);

/**
 * @brief Retrieve the latest feedback sample for an axis.
 *
 * @param master Master context.
 * @param axis Axis index.
 * @return Feedback structure pointer.
 */
const ethcat_axis_feedback_t *ethcat_master_get_feedback(const ethcat_master_t *master,
                                                         size_t axis);

#endif /* ETHCAT_ETHCAT_H */
