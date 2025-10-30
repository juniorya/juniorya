/**
 * @file st_runtime.h
 * @brief Runtime scaffolding for Structured Text programs in the IDE.
 */

#ifndef ST_RUNTIME_H
#define ST_RUNTIME_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Execution context exposed to generated C programs.
 */
typedef struct
{
    float cycle_time_ms;  /**< Cycle time provided by runtime. */
    float inputs[16];     /**< Normalised input channels. */
    float outputs[16];    /**< Output channels to be written by program. */
} st_runtime_context;

#ifdef __cplusplus
}
#endif

#endif
