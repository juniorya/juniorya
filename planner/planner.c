#include "planner.h"
#include <stddef.h>
#include "utils/fixed.h"

static uint16_t planner_count(const planner_queue_t *planner)
{
    if (planner->head >= planner->tail) {
        return (uint16_t)(planner->head - planner->tail);
    }
    return (uint16_t)(PLANNER_QUEUE_LENGTH - planner->tail + planner->head);
}

void planner_init(planner_queue_t *planner, uint32_t control_period_us)
{
    planner->head = planner->tail = 0U;
    planner->control_period_us = control_period_us;
    for (int i = 0; i < 3; ++i) {
        planner->current_pose.xyz[i] = 0;
    }
}

bool planner_is_empty(const planner_queue_t *planner)
{
    return planner->head == planner->tail;
}

static bool planner_full(const planner_queue_t *planner)
{
    return ((planner->head + 1U) % PLANNER_QUEUE_LENGTH) == planner->tail;
}

static void planner_recalculate(planner_queue_t *planner)
{
    uint16_t count = planner_count(planner);
    if (count < 2U) {
        return;
    }
    uint16_t idx = planner->tail;
    planner_block_t *prev = NULL;
    for (uint16_t i = 0; i < count; ++i) {
        planner_block_t *block = &planner->blocks[idx];
        if (prev != NULL) {
            q16_16_t diff_prev[3];
            q16_16_t diff_curr[3];
            for (int axis = 0; axis < 3; ++axis) {
                diff_prev[axis] = prev->end.xyz[axis] - prev->start.xyz[axis];
                diff_curr[axis] = block->end.xyz[axis] - block->start.xyz[axis];
            }
            q16_16_t dot = 0;
            q16_16_t mag_prev = 0;
            q16_16_t mag_curr = 0;
            for (int axis = 0; axis < 3; ++axis) {
                dot += q16_16_mul(diff_prev[axis], diff_curr[axis]);
                mag_prev += q16_16_mul(diff_prev[axis], diff_prev[axis]);
                mag_curr += q16_16_mul(diff_curr[axis], diff_curr[axis]);
            }
            q16_16_t denom = q16_16_mul(q16_16_sqrt(mag_prev), q16_16_sqrt(mag_curr));
            q16_16_t cos_theta = denom != 0 ? q16_16_div(dot, denom) : Q16_16_ONE;
            q16_16_t smoothing = q16_16_clamp(q16_16_from_float(0.5f * (1.0f - q16_16_to_float(cos_theta))), 0, Q16_16_ONE);
            q16_16_t reduced = q16_16_mul(prev->feedrate, Q16_16_ONE - smoothing);
            prev->exit_velocity = reduced;
            block->entry_velocity = reduced;
        }
        prev = block;
        idx = (uint16_t)((idx + 1U) % PLANNER_QUEUE_LENGTH);
    }
}

bool planner_push_line(planner_queue_t *planner, const delta_pose_t *target, q16_16_t feedrate, q16_16_t accel, q16_16_t jerk)
{
    if (planner_full(planner)) {
        return false;
    }
    planner_block_t *block = &planner->blocks[planner->head];
    block->start = planner->current_pose;
    block->end = *target;
    block->feedrate = feedrate;
    block->entry_velocity = feedrate;
    block->exit_velocity = feedrate;
    block->accel = accel;
    block->jerk = jerk;
    block->tick_index = 0U;
    block->active = false;

    q16_16_t diff_sq = 0;
    for (int axis = 0; axis < 3; ++axis) {
        q16_16_t diff = block->end.xyz[axis] - block->start.xyz[axis];
        diff_sq += q16_16_mul(diff, diff);
    }
    q16_16_t distance = q16_16_sqrt(diff_sq);
    if (distance == 0) {
        block->total_ticks = 1U;
    } else {
        float period = (float)planner->control_period_us / 1000000.0f;
        float time_s = q16_16_to_float(distance) / q16_16_to_float(feedrate);
        uint32_t ticks = (uint32_t)(time_s / period);
        if (ticks == 0U) {
            ticks = 1U;
        }
        block->total_ticks = ticks;
    }

    planner->head = (uint16_t)((planner->head + 1U) % PLANNER_QUEUE_LENGTH);
    planner_recalculate(planner);
    planner->current_pose = block->end;
    return true;
}

bool planner_step(planner_queue_t *planner, delta_pose_t *pose_out)
{
    if (planner_is_empty(planner)) {
        *pose_out = planner->current_pose;
        return false;
    }
    planner_block_t *block = &planner->blocks[planner->tail];
    if (!block->active) {
        block->active = true;
        block->tick_index = 0U;
    }
    uint32_t index = block->tick_index;
    uint32_t total = block->total_ticks;
    q16_16_t u = (q16_16_t)((index << 16) / (total == 0U ? 1U : total));
    q16_16_t progress;
    bool low_buffer = planner_count(planner) < 2U;
    if (low_buffer) {
        progress = u;
    } else {
        q16_16_t u2 = q16_16_mul(u, u);
        q16_16_t u3 = q16_16_mul(u2, u);
        progress = q16_16_mul(q16_16_from_int(3), u2) - q16_16_mul(q16_16_from_int(2), u3);
    }

    for (int axis = 0; axis < 3; ++axis) {
        q16_16_t diff = block->end.xyz[axis] - block->start.xyz[axis];
        pose_out->xyz[axis] = block->start.xyz[axis] + q16_16_mul(diff, progress);
    }

    block->tick_index++;
    if (block->tick_index >= block->total_ticks) {
        planner->current_pose = block->end;
        planner->tail = (uint16_t)((planner->tail + 1U) % PLANNER_QUEUE_LENGTH);
    }
    return true;
}

void planner_hold(planner_queue_t *planner)
{
    planner->head = planner->tail;
}
