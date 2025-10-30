/**
 * @file lookahead.c
 * @brief Simple ring buffer implementation for spline preview.
 */

#include "lookahead.h"

#include <stdlib.h>
#include <string.h>

void lookahead_init(lookahead_buffer *buf, size_t capacity)
{
    buf->buffer = (spl_plan_t *)calloc(capacity, sizeof(spl_plan_t));
    buf->capacity = capacity;
    buf->head = 0U;
    buf->tail = 0U;
    buf->count = 0U;
}

void lookahead_deinit(lookahead_buffer *buf)
{
    free(buf->buffer);
    buf->buffer = NULL;
    buf->capacity = 0U;
    buf->head = buf->tail = buf->count = 0U;
}

bool lookahead_push(lookahead_buffer *buf, const spl_plan_t *plan)
{
    if (buf->count >= buf->capacity)
    {
        return false;
    }
    buf->buffer[buf->head] = *plan;
    buf->head = (buf->head + 1U) % buf->capacity;
    buf->count++;
    return true;
}

bool lookahead_pop(lookahead_buffer *buf, spl_plan_t *plan)
{
    if (buf->count == 0U)
    {
        return false;
    }
    *plan = buf->buffer[buf->tail];
    buf->tail = (buf->tail + 1U) % buf->capacity;
    buf->count--;
    return true;
}

void lookahead_clear(lookahead_buffer *buf)
{
    buf->head = buf->tail = buf->count = 0U;
    memset(buf->buffer, 0, buf->capacity * sizeof(spl_plan_t));
}
