/**
 * @file state_machine.h
 * @brief Finite-state machine driving CNC operations.
 */

#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <stdbool.h>

#include "core/cnc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Step the CNC state machine.
 * @param cnc CNC context.
 */
void state_machine_step(cnc_context *cnc);

/**
 * @brief Query whether main loop should continue.
 * @param cnc CNC context.
 * @return True while CNC should execute.
 */
bool state_machine_should_run(const cnc_context *cnc);

#ifdef __cplusplus
}
#endif

#endif
