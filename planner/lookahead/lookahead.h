/**
 * @file lookahead.h
 * @brief Look-ahead buffer for multi-segment preview.
 */

#ifndef LOOKAHEAD_H
#define LOOKAHEAD_H

#include <stdbool.h>
#include <stddef.h>

#include "planner/splines/splines.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Look-ahead queue. */
typedef struct
{
    spl_plan_t *buffer; /**< Ring buffer of spline plans. */
    size_t capacity;    /**< Buffer capacity. */
    size_t head;        /**< Head index. */
    size_t tail;        /**< Tail index. */
    size_t count;       /**< Occupancy. */
} lookahead_buffer;

/**
 * @brief Initialise look-ahead buffer.
 * @param buf Buffer instance.
 * @param capacity Desired capacity.
 */
void lookahead_init(lookahead_buffer *buf, size_t capacity);

/**
 * @brief Release look-ahead buffer resources.
 * @param buf Buffer instance.
 */
void lookahead_deinit(lookahead_buffer *buf);

/**
 * @brief Push spline plan into queue.
 * @param buf Buffer instance.
 * @param plan Plan to push.
 * @return True if accepted.
 */
bool lookahead_push(lookahead_buffer *buf, const spl_plan_t *plan);

/**
 * @brief Pop oldest spline plan.
 * @param buf Buffer instance.
 * @param plan Output plan.
 * @return True if plan was available.
 */
bool lookahead_pop(lookahead_buffer *buf, spl_plan_t *plan);

/**
 * @brief Clear queue contents.
 * @param buf Buffer instance.
 */
void lookahead_clear(lookahead_buffer *buf);

#ifdef __cplusplus
}
#endif

#endif
