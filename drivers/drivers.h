/**
 * @file drivers.h
 * @brief Hardware drivers for safety IO and watchdog.
 */

#ifndef DRIVERS_H
#define DRIVERS_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Driver context containing IO state. */
typedef struct
{
    bool estop_active; /**< Emergency stop signal. */
    bool limits_ok;    /**< Limit switches status. */
} drivers_context;

/**
 * @brief Initialise hardware drivers.
 * @param ctx Driver context.
 * @return True on success.
 */
bool drivers_init(drivers_context *ctx);

/**
 * @brief Poll IO and refresh state.
 * @param ctx Driver context.
 */
void drivers_update(drivers_context *ctx);

/**
 * @brief Shutdown drivers.
 * @param ctx Driver context.
 */
void drivers_shutdown(drivers_context *ctx);

#ifdef __cplusplus
}
#endif

#endif
