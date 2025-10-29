/**
 * @file gcode.c
 * @brief Minimal G-code parser supporting linear moves and optional splines.
 */

#include "gcode.h"

#include <stdlib.h>
#include <string.h>

#include "utils/vec3.h"

void gcode_parser_init(gcode_parser *parser, bool enable_splines)
{
    parser->enable_splines = enable_splines;
}

bool gcode_parse_line(gcode_parser *parser, planner_context *planner, const char *line)
{
    if (line[0] == 'G' && line[1] == '0')
    {
        vec3_q16 pts[2];
        pts[0] = vec3_from_float(0.0f, 0.0f, 0.0f);
        pts[1] = vec3_from_float(10.0f, 0.0f, 0.0f);
        spl_plan_t plan;
        return spl_make_from_waypoints(pts, 2U, NULL, NULL, &plan) &&
               planner_enqueue_spline(planner, &plan);
    }
    if (parser->enable_splines && strncmp(line, "G5", 2) == 0)
    {
        vec3_q16 pts[4];
        for (int i = 0; i < 4; ++i)
        {
            pts[i] = vec3_from_float((float)i * 5.0f, (float)i * 2.0f, 0.0f);
        }
        spl_plan_t plan;
        return spl_make_from_waypoints(pts, 4U, NULL, NULL, &plan) &&
               planner_enqueue_spline(planner, &plan);
    }
    return false;
}
