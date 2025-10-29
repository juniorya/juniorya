#ifndef PLANNER_PLANNER_H
#define PLANNER_PLANNER_H

#include <stdbool.h>
#include <stdint.h>
#include "kinematics/delta.h"
#include "utils/fixed.h"

#define PLANNER_QUEUE_LENGTH 128

typedef struct {
    delta_pose_t start;
    delta_pose_t end;
    q16_16_t feedrate;
    q16_16_t entry_velocity;
    q16_16_t exit_velocity;
    q16_16_t accel;
    q16_16_t jerk;
    uint32_t total_ticks;
    uint32_t tick_index;
    bool active;
} planner_block_t;

typedef struct {
    planner_block_t blocks[PLANNER_QUEUE_LENGTH];
    uint16_t head;
    uint16_t tail;
    delta_pose_t current_pose;
    uint32_t control_period_us;
} planner_queue_t;

void planner_init(planner_queue_t *planner, uint32_t control_period_us);
bool planner_is_empty(const planner_queue_t *planner);
bool planner_push_line(planner_queue_t *planner, const delta_pose_t *target, q16_16_t feedrate, q16_16_t accel, q16_16_t jerk);
bool planner_step(planner_queue_t *planner, delta_pose_t *pose_out);
void planner_hold(planner_queue_t *planner);

#endif
