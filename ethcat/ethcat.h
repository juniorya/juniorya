/**
 * @file ethcat.h
 * @brief EtherCAT master abstraction with DC/Sync0 support.
 */

#ifndef ETHCAT_H
#define ETHCAT_H

#include <stdbool.h>
#include <stddef.h>

#include "cia402/cia402.h"
#include "utils/q16.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Maximum axes supported by the simulated master. */
#define ETHCAT_MAX_AXES 8U

/** @brief EtherCAT master configuration. */
typedef struct
{
    char name[64];          /**< Drive identifier extracted from XML. */
    uint32_t vendor_id;     /**< Vendor ID. */
    uint32_t product_code;  /**< Product code. */
    uint8_t revision;       /**< Revision identifier. */
} ethcat_drive_descriptor;

typedef struct
{
    q16_16 sync_period_us;                 /**< Sync0 period in microseconds. */
    size_t axis_count;                     /**< Number of axes. */
    ethcat_drive_descriptor drives[ETHCAT_MAX_AXES]; /**< Drive descriptors loaded from XML. */
    size_t drive_count;                    /**< Number of valid descriptors. */
} ethcat_master_config;

/** @brief Runtime state of EtherCAT master. */
typedef struct
{
    ethcat_master_config config;                   /**< Static configuration. */
    cia402_mode mode[ETHCAT_MAX_AXES];             /**< Operation mode per axis. */
    q16_16 target_pos[ETHCAT_MAX_AXES];            /**< Target positions for axes. */
    q16_16 target_vel[ETHCAT_MAX_AXES];            /**< Target velocities for axes. */
    q16_16 target_torque[ETHCAT_MAX_AXES];         /**< Target torques for axes. */
    q16_16 actual_pos[ETHCAT_MAX_AXES];            /**< Simulated feedback position. */
    q16_16 actual_vel[ETHCAT_MAX_AXES];            /**< Simulated feedback velocity. */
    q16_16 actual_torque[ETHCAT_MAX_AXES];         /**< Simulated feedback torque. */
} ethcat_master;

/**
 * @brief Fill EtherCAT configuration with defaults.
 *
 * @param cfg Configuration structure.
 */
void ethcat_master_default_config(ethcat_master_config *cfg);

/**
 * @brief Initialise EtherCAT master.
 *
 * @param master Master instance.
 * @param cfg Configuration parameters.
 * @return True on success.
 */
bool ethcat_master_init(ethcat_master *master, const ethcat_master_config *cfg);

/**
 * @brief Update EtherCAT cycle (TxPDO/RxPDO exchange).
 *
 * @param master Master instance.
 */
void ethcat_master_update(ethcat_master *master);

/**
 * @brief Select CiA-402 operation mode for an axis.
 *
 * @param master Master instance.
 * @param axis Axis index.
 * @param mode Requested operation mode.
 */
void ethcat_master_set_operation_mode(ethcat_master *master, size_t axis, cia402_mode mode);

/**
 * @brief Set position target for an axis.
 *
 * @param master Master instance.
 * @param axis Axis index.
 * @param target Target position in Q16.16 workspace units.
 */
void ethcat_master_set_position(ethcat_master *master, size_t axis, q16_16 target);

/**
 * @brief Set velocity target for an axis.
 *
 * @param master Master instance.
 * @param axis Axis index.
 * @param target Velocity demand.
 */
void ethcat_master_set_velocity(ethcat_master *master, size_t axis, q16_16 target);

/**
 * @brief Set torque target for an axis.
 *
 * @param master Master instance.
 * @param axis Axis index.
 * @param target Torque demand.
 */
void ethcat_master_set_torque(ethcat_master *master, size_t axis, q16_16 target);

/**
 * @brief Compatibility wrapper for legacy position setter.
 *
 * @param master Master instance.
 * @param axis Axis index.
 * @param target Target position.
 */
void ethcat_master_set_target(ethcat_master *master, size_t axis, q16_16 target);

/**
 * @brief Query actual position feedback.
 *
 * @param master Master instance.
 * @param axis Axis index.
 * @return Actual position.
 */
q16_16 ethcat_master_get_actual_position(const ethcat_master *master, size_t axis);

/**
 * @brief Query actual velocity feedback.
 *
 * @param master Master instance.
 * @param axis Axis index.
 * @return Actual velocity.
 */
q16_16 ethcat_master_get_actual_velocity(const ethcat_master *master, size_t axis);

/**
 * @brief Query actual torque feedback.
 *
 * @param master Master instance.
 * @param axis Axis index.
 * @return Actual torque.
 */
q16_16 ethcat_master_get_actual_torque(const ethcat_master *master, size_t axis);

/**
 * @brief Shut down master and release resources.
 *
 * @param master Master instance.
 */
void ethcat_master_shutdown(ethcat_master *master);

#ifdef __cplusplus
}
#endif

#endif
