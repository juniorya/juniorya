#include "parser.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static float q_to_float(q16_16_t v)
{
    return q16_16_to_float(v);
}

static q16_16_t float_to_q(float v)
{
    return q16_16_from_float(v);
}

void gcode_parser_init(gcode_parser_t *parser)
{
    parser->absolute_positioning = true;
    parser->units_inch = false;
    parser->current_feedrate = q16_16_from_float(50.0f);
    parser->last_dwell_ms = 0;
    for (int i = 0; i < 3; ++i) {
        parser->current_pose.xyz[i] = 0;
    }
}

static float parse_float(const char *str, int *index)
{
    char buffer[16];
    int len = 0;
    int i = *index;
    while (str[i] != '\0' && (isdigit((unsigned char)str[i]) || str[i] == '.' || str[i] == '-' || str[i] == '+')) {
        if (len < (int)sizeof(buffer) - 1) {
            buffer[len++] = str[i];
        }
        ++i;
    }
    buffer[len] = '\0';
    *index = i;
    return strtof(buffer, NULL);
}

static q16_16_t convert_units(const gcode_parser_t *parser, float value)
{
    float scale = parser->units_inch ? 25.4f : 1.0f;
    return float_to_q(value * scale);
}

gcode_event_t gcode_parser_process_line(gcode_parser_t *parser, const char *line, planner_queue_t *planner)
{
    int g_code = -1;
    int m_code = -1;
    float values[8] = {0};
    bool has_value[8] = {false};
    float arc_offset[3] = {0};
    bool has_arc_offset[3] = {false};
    float dwell_time = 0.0f;
    float units_scale = parser->units_inch ? 25.4f : 1.0f;

    for (int i = 0; line[i] != '\0';) {
        if (isspace((unsigned char)line[i])) {
            ++i;
            continue;
        }
        char letter = (char)toupper((unsigned char)line[i]);
        ++i;
        if (letter == 'G') {
            g_code = (int)parse_float(line, &i);
        } else if (letter == 'M') {
            m_code = (int)parse_float(line, &i);
        } else if (letter == 'X') {
            values[0] = parse_float(line, &i);
            has_value[0] = true;
        } else if (letter == 'Y') {
            values[1] = parse_float(line, &i);
            has_value[1] = true;
        } else if (letter == 'Z') {
            values[2] = parse_float(line, &i);
            has_value[2] = true;
        } else if (letter == 'I') {
            arc_offset[0] = parse_float(line, &i);
            has_arc_offset[0] = true;
        } else if (letter == 'J') {
            arc_offset[1] = parse_float(line, &i);
            has_arc_offset[1] = true;
        } else if (letter == 'K') {
            arc_offset[2] = parse_float(line, &i);
            has_arc_offset[2] = true;
        } else if (letter == 'F') {
            values[3] = parse_float(line, &i);
            has_value[3] = true;
        } else if (letter == 'S') {
            values[4] = parse_float(line, &i);
            has_value[4] = true;
        } else if (letter == 'P') {
            dwell_time = parse_float(line, &i);
        } else {
            while (line[i] != '\0' && !isspace((unsigned char)line[i])) {
                ++i;
            }
        }
    }

    if (has_value[3]) {
        parser->current_feedrate = convert_units(parser, values[3] / 60.0f);
    }

    switch (g_code) {
    case 0:
    case 1: {
        delta_pose_t target = parser->current_pose;
        for (int axis = 0; axis < 3; ++axis) {
            if (has_value[axis]) {
                q16_16_t delta = convert_units(parser, values[axis]);
                if (parser->absolute_positioning) {
                    target.xyz[axis] = delta;
                } else {
                    target.xyz[axis] += delta;
                }
            }
        }
        planner_push_line(planner, &target, parser->current_feedrate, q16_16_from_float(1.0f), q16_16_from_float(5.0f));
        parser->current_pose = target;
        return GCODE_EVENT_NONE;
    }
    case 2:
    case 3: {
        float dir = (g_code == 2) ? -1.0f : 1.0f;
        float start_x = q_to_float(parser->current_pose.xyz[0]);
        float start_y = q_to_float(parser->current_pose.xyz[1]);
        float end_x = start_x;
        float end_y = start_y;
        if (has_value[0]) {
            float val = values[0] * units_scale;
            end_x = parser->absolute_positioning ? val : start_x + val;
        }
        if (has_value[1]) {
            float val = values[1] * units_scale;
            end_y = parser->absolute_positioning ? val : start_y + val;
        }
        float center_x = start_x + (has_arc_offset[0] ? arc_offset[0] * units_scale : 0.0f);
        float center_y = start_y + (has_arc_offset[1] ? arc_offset[1] * units_scale : 0.0f);
        float radius = hypotf(start_x - center_x, start_y - center_y);
        float start_angle = atan2f(start_y - center_y, start_x - center_x);
        float end_angle = atan2f(end_y - center_y, end_x - center_x);
        float sweep = end_angle - start_angle;
        if (dir > 0.0f && sweep < 0.0f) {
            sweep += 2.0f * (float)M_PI;
        } else if (dir < 0.0f && sweep > 0.0f) {
            sweep -= 2.0f * (float)M_PI;
        }
        int segments = (int)(fabsf(sweep) / (float)(5.0f * (M_PI / 180.0f)));
        if (segments < 1) {
            segments = 1;
        }
        for (int seg = 1; seg <= segments; ++seg) {
            float angle = start_angle + sweep * ((float)seg / (float)segments);
            delta_pose_t target = parser->current_pose;
            target.xyz[0] = float_to_q(center_x + cosf(angle) * radius);
            target.xyz[1] = float_to_q(center_y + sinf(angle) * radius);
            planner_push_line(planner, &target, parser->current_feedrate, q16_16_from_float(1.0f), q16_16_from_float(5.0f));
            parser->current_pose = target;
        }
        return GCODE_EVENT_NONE;
    }
    case 4:
        parser->last_dwell_ms = float_to_q(dwell_time * 1000.0f);
        return GCODE_EVENT_DWELL;
    case 20:
        parser->units_inch = true;
        return GCODE_EVENT_NONE;
    case 21:
        parser->units_inch = false;
        return GCODE_EVENT_NONE;
    case 90:
        parser->absolute_positioning = true;
        return GCODE_EVENT_NONE;
    case 91:
        parser->absolute_positioning = false;
        return GCODE_EVENT_NONE;
    default:
        break;
    }

    switch (m_code) {
    case 17:
        return GCODE_EVENT_ENABLE_DRIVES;
    case 18:
        return GCODE_EVENT_DISABLE_DRIVES;
    case 112:
        return GCODE_EVENT_ESTOP;
    default:
        break;
    }

    if (g_code == -1 && m_code == -1 && line[0] == '$') {
        if (strncmp(line, "$H", 2) == 0) {
            parser->current_pose.xyz[0] = parser->current_pose.xyz[1] = parser->current_pose.xyz[2] = 0;
        }
    }

    return GCODE_EVENT_NONE;
}
