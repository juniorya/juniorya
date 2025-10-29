/**
 * @file drivers.h
 * @brief Hardware abstraction for safety IO.
 */
#ifndef DRIVERS_DRIVERS_H
#define DRIVERS_DRIVERS_H

#include <stdbool.h>

/**
 * @brief Initialize IO drivers.
 *
 * @return true on success.
 */
bool drivers_init(void);

/**
 * @brief Check whether emergency stop is active.
 *
 * @return true if pressed.
 */
bool drivers_estop_pressed(void);

#endif /* DRIVERS_DRIVERS_H */
