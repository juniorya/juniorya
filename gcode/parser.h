#ifndef GCODE_PARSER_H
#define GCODE_PARSER_H

#include <stdbool.h>
#include "planner/planner.h"

typedef enum {
    GCODE_EVENT_NONE = 0,
    GCODE_EVENT_ENABLE_DRIVES,
    GCODE_EVENT_DISABLE_DRIVES,
    GCODE_EVENT_ESTOP,
    GCODE_EVENT_DWELL
} gcode_event_t;

typedef struct {
    bool absolute_positioning;
    bool units_inch;
    q16_16_t current_feedrate;
    delta_pose_t current_pose;
    q16_16_t last_dwell_ms;
} gcode_parser_t;

void gcode_parser_init(gcode_parser_t *parser);
gcode_event_t gcode_parser_process_line(gcode_parser_t *parser, const char *line, planner_queue_t *planner);

#endif
