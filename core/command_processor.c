#include "command_processor.h"
#include <string.h>

static bool queue_full(const command_queue_t *queue)
{
    return ((queue->head + 1U) % COMMAND_QUEUE_LENGTH) == queue->tail;
}

void command_queue_init(command_queue_t *queue)
{
    queue->head = queue->tail = 0U;
}

bool command_queue_enqueue(command_queue_t *queue, const char *line)
{
    if (queue_full(queue)) {
        return false;
    }
    strncpy(queue->lines[queue->head], line, COMMAND_MAX_LENGTH - 1);
    queue->lines[queue->head][COMMAND_MAX_LENGTH - 1] = '\0';
    queue->head = (uint16_t)((queue->head + 1U) % COMMAND_QUEUE_LENGTH);
    return true;
}

static bool queue_empty(const command_queue_t *queue)
{
    return queue->head == queue->tail;
}

static const char *queue_front(command_queue_t *queue)
{
    if (queue_empty(queue)) {
        return NULL;
    }
    return queue->lines[queue->tail];
}

static void queue_pop(command_queue_t *queue)
{
    if (!queue_empty(queue)) {
        queue->tail = (uint16_t)((queue->tail + 1U) % COMMAND_QUEUE_LENGTH);
    }
}

bool command_processor_step(command_queue_t *queue, cnc_runtime_t *runtime, gcode_parser_t *parser, planner_queue_t *planner, cia402_axis_t *axes)
{
    if (queue_empty(queue)) {
        return false;
    }
    const char *line = queue_front(queue);
    gcode_event_t event = gcode_parser_process_line(parser, line, planner);

    switch (event) {
    case GCODE_EVENT_ENABLE_DRIVES:
        runtime->drives_enabled = true;
        runtime->state = CNC_STATE_RUN;
        for (int axis = 0; axis < 3; ++axis) {
            axes[axis].quick_stop = false;
            axes[axis].halt = false;
            cia402_axis_fault_reset(&axes[axis]);
        }
        break;
    case GCODE_EVENT_DISABLE_DRIVES:
        runtime->drives_enabled = false;
        runtime->state = CNC_STATE_HOLD;
        for (int axis = 0; axis < 3; ++axis) {
            axes[axis].quick_stop = true;
        }
        break;
    case GCODE_EVENT_ESTOP:
        cnc_runtime_set_state(runtime, CNC_STATE_ESTOP);
        for (int axis = 0; axis < 3; ++axis) {
            axes[axis].quick_stop = true;
        }
        break;
    case GCODE_EVENT_DWELL:
        runtime->state = CNC_STATE_HOLD;
        break;
    case GCODE_EVENT_NONE:
    default:
        runtime->state = CNC_STATE_RUN;
        break;
    }

    queue_pop(queue);
    return true;
}
