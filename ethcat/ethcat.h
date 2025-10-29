/**
 * @file ethcat.h
 * @brief EtherCAT master abstraction with DC/Sync0 support.
 */

#ifndef ETHCAT_H
#define ETHCAT_H

#include <stdbool.h>
#include <stddef.h>

#include "utils/q16.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief EtherCAT master configuration. */
typedef struct
{
    q16_16 sync_period_us; /**< Sync0 period in microseconds. */
    size_t axis_count;     /**< Number of axes. */
} ethcat_master_config;

/** @brief Runtime state of EtherCAT master. */
typedef struct
{
    ethcat_master_config config; /**< Static configuration. */
    q16_16 target_pos[3];        /**< Target positions for axes. */
    q16_16 actual_pos[3];        /**< Simulated feedback. */
} ethcat_master;

/**
 * @brief Fill EtherCAT configuration with defaults.
 * @param cfg Configuration structure.
 */
void ethcat_master_default_config(ethcat_master_config *cfg);

/**
 * @brief Initialise EtherCAT master.
 * @param master Master instance.
 * @param cfg Configuration parameters.
 * @return True on success.
 */
bool ethcat_master_init(ethcat_master *master, const ethcat_master_config *cfg);

/**
 * @brief Update EtherCAT cycle (TxPDO/RxPDO exchange).
 * @param master Master instance.
 */
void ethcat_master_update(ethcat_master *master);

/**
 * @brief Set CSP target position for an axis.
 * @param master Master instance.
 * @param axis Axis index.
 * @param target Target position in Q16.16 workspace units.
 */
void ethcat_master_set_target(ethcat_master *master, size_t axis, q16_16 target);

/**
 * @brief Shut down master and release resources.
 * @param master Master instance.
 */
void ethcat_master_shutdown(ethcat_master *master);

#ifdef __cplusplus
}
#endif

#endif
