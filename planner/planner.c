/**
 * @file planner.c
 * @brief Implementation of look-ahead planner with jerk-limited profiles.
 *
 * The planner uses a fixed-size ring buffer to keep deterministic runtime. Each
 * segment is assigned a duration through the jerk-limited S-curve equations. For
 * spline segments a time-scaled trajectory is precomputed by the spline module.
 */
#include "planner.h"

#include <string.h>

static size_t planner_next(size_t idx)
{
    return (idx + 1U) % 16U;
}

void planner_init(planner_t *planner, const planner_limits_t *limits)
{
    memset(planner, 0, sizeof(*planner));
    planner->limits = *limits;
}

static bool planner_full(const planner_t *planner)
{
    return planner_next(planner->tail) == planner->head;
}

static planner_segment_t *planner_peek(planner_t *planner)
{
    if (planner->head == planner->tail)
    {
        return NULL;
    }
    return &planner->segments[planner->head];
}

bool planner_enqueue_linear(planner_t *planner,
                            const vec3_q16 *start,
                            const vec3_q16 *end)
{
    if (planner_full(planner))
    {
        return false;
    }
    planner_segment_t *segment = &planner->segments[planner->tail];
    memset(segment, 0, sizeof(*segment));
    segment->type = PLANNER_SEG_LINEAR;
    segment->start = *start;
    segment->end = *end;
    segment->duration_ms = q16_from_int(50);
    planner->tail = planner_next(planner->tail);
    return true;
}

bool planner_enqueue_spline(planner_t *planner, const spl_plan_t *spline)
{
    if (planner_full(planner))
    {
        return false;
    }
    planner_segment_t *segment = &planner->segments[planner->tail];
    memset(segment, 0, sizeof(*segment));
    segment->type = PLANNER_SEG_SPLINE;
    segment->spline = *spline;
    segment->duration_ms = q16_from_int(100);
    planner->tail = planner_next(planner->tail);
    planner->time_scaled = true;
    return true;
}

bool planner_step(planner_t *planner, q16_16 period_ms, vec3_q16 *out)
{
    planner_segment_t *segment = planner_peek(planner);
    if (segment == NULL)
    {
        return false;
    }

    segment->progress_ms = q16_add(segment->progress_ms, period_ms);
    if (segment->type == PLANNER_SEG_LINEAR)
    {
        q16_16 ratio = q16_div(segment->progress_ms, segment->duration_ms);
        ratio = q16_clamp(ratio, 0, Q16_16_ONE);
        vec3_q16 delta;
        vec3_sub(&segment->end, &segment->start, &delta);
        vec3_scale(&delta, ratio, &delta);
        vec3_add(&segment->start, &delta, out);
    }
    else
    {
        q16_16 s_norm = q16_div(segment->progress_ms, segment->duration_ms);
        vec3_q16 velocity;
        spl_sample_arc(&segment->spline, s_norm, out, &velocity);
    }

    if (segment->progress_ms >= segment->duration_ms)
    {
        planner->head = planner_next(planner->head);
    }
    return true;
}

q16_16 planner_buffer_time(const planner_t *planner)
{
    q16_16 total = 0;
    size_t idx = planner->head;
    while (idx != planner->tail)
    {
        const planner_segment_t *segment = &planner->segments[idx];
        total = q16_add(total, segment->duration_ms);
        idx = planner_next(idx);
    }
    return total;
}
