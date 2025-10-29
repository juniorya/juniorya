#ifndef CORE_COMMAND_PROCESSOR_H
#define CORE_COMMAND_PROCESSOR_H

#include <stdbool.h>
#include "core/cnc_state.h"
#include "gcode/parser.h"
#include "motion/motion_control.h"

#define COMMAND_QUEUE_LENGTH 128
#define COMMAND_MAX_LENGTH 96

typedef struct {
    char lines[COMMAND_QUEUE_LENGTH][COMMAND_MAX_LENGTH];
    uint16_t head;
    uint16_t tail;
} command_queue_t;

void command_queue_init(command_queue_t *queue);
bool command_queue_enqueue(command_queue_t *queue, const char *line);
bool command_processor_step(command_queue_t *queue, cnc_runtime_t *runtime, gcode_parser_t *parser, planner_queue_t *planner, cia402_axis_t *axes);

#endif
