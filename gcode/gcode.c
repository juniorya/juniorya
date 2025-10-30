/**
 * @file gcode.c
 * @brief Minimal G-code parser supporting linear moves and optional splines.
 */

#include "gcode.h"

#include <ctype.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "planner/splines/splines.h"
#include "utils/q16.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef struct
{
    double x;
    double y;
    double z;
    double i;
    double j;
    double k;
    double r;
    double p;
    double q;
    double feed;
    double dwell;
    double spindle;
    bool has_x;
    bool has_y;
    bool has_z;
    bool has_i;
    bool has_j;
    bool has_k;
    bool has_r;
    bool has_p;
    bool has_q;
    bool has_feed;
    bool has_dwell;
    bool has_spindle;
    double g_codes[4];
    size_t g_count;
    int m_codes[4];
    size_t m_count;
} gcode_modal_tokens;

static float unit_scale(const gcode_parser *parser)
{
    return parser->metric_units ? 1.0f : 25.4f;
}

static void set_report(gcode_parser *parser, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    (void)vsnprintf(parser->last_report, sizeof(parser->last_report), fmt, args);
    va_end(args);
}

static void parser_apply_position(gcode_parser *parser, const vec3_q16 *new_pos)
{
    parser->position = *new_pos;
}

static bool parse_value(const char **cursor, double *out)
{
    char *end = NULL;
    double val = strtod(*cursor, &end);
    if (end == *cursor)
    {
        return false;
    }
    *out = val;
    *cursor = end;
    return true;
}

static void collect_token(gcode_modal_tokens *tokens, char code, double value)
{
    switch (code)
    {
        case 'X':
            tokens->x = value;
            tokens->has_x = true;
            break;
        case 'Y':
            tokens->y = value;
            tokens->has_y = true;
            break;
        case 'Z':
            tokens->z = value;
            tokens->has_z = true;
            break;
        case 'I':
            tokens->i = value;
            tokens->has_i = true;
            break;
        case 'J':
            tokens->j = value;
            tokens->has_j = true;
            break;
        case 'K':
            tokens->k = value;
            tokens->has_k = true;
            break;
        case 'R':
            tokens->r = value;
            tokens->has_r = true;
            break;
        case 'P':
            tokens->p = value;
            tokens->has_p = true;
            break;
        case 'Q':
            tokens->q = value;
            tokens->has_q = true;
            break;
        case 'F':
            tokens->feed = value;
            tokens->has_feed = true;
            break;
        case 'S':
            tokens->spindle = value;
            tokens->has_spindle = true;
            break;
        case 'G':
            if (tokens->g_count < (sizeof(tokens->g_codes) / sizeof(tokens->g_codes[0])))
            {
                tokens->g_codes[tokens->g_count++] = value;
            }
            break;
        case 'M':
            if (tokens->m_count < (sizeof(tokens->m_codes) / sizeof(tokens->m_codes[0])))
            {
                tokens->m_codes[tokens->m_count++] = (int)value;
            }
            break;
        case 'D':
            tokens->dwell = value;
            tokens->has_dwell = true;
            break;
        default:
            break;
    }
}

static void strip_inline_comment(const char **cursor)
{
    while ((**cursor != '\0') && (**cursor != ')'))
    {
        (*cursor)++;
    }
    if (**cursor == ')')
    {
        (*cursor)++;
    }
}

static void tokenise_line(const char *line, gcode_modal_tokens *tokens)
{
    memset(tokens, 0, sizeof(*tokens));
    const char *ptr = line;
    while (*ptr != '\0')
    {
        if (*ptr == ';')
        {
            break;
        }
        if (*ptr == '(')
        {
            ptr++;
            strip_inline_comment(&ptr);
            continue;
        }
        if (isspace((unsigned char)*ptr))
        {
            ptr++;
            continue;
        }
        char code = (char)toupper((unsigned char)*ptr);
        ptr++;
        double value = 0.0;
        if (!parse_value(&ptr, &value))
        {
            continue;
        }
        collect_token(tokens, code, value);
    }
}

static vec3_q16 make_target_position(const gcode_parser *parser, const gcode_modal_tokens *tokens)
{
    float scale = unit_scale(parser);
    vec3_q16 target = parser->position;
    if (tokens->has_x)
    {
        float vx = (float)tokens->x * scale;
        q16_16 qx = q16_from_float(vx);
        target.x = parser->absolute_mode ? qx : q16_add(target.x, qx);
    }
    if (tokens->has_y)
    {
        float vy = (float)tokens->y * scale;
        q16_16 qy = q16_from_float(vy);
        target.y = parser->absolute_mode ? qy : q16_add(target.y, qy);
    }
    if (tokens->has_z)
    {
        float vz = (float)tokens->z * scale;
        q16_16 qz = q16_from_float(vz);
        target.z = parser->absolute_mode ? qz : q16_add(target.z, qz);
    }
    return target;
}

static bool enqueue_linear(gcode_parser *parser,
                           planner_context *planner,
                           const vec3_q16 *target)
{
    vec3_q16 pts[2];
    pts[0] = parser->position;
    pts[1] = *target;
    spl_plan_t plan;
    if (!spl_make_from_waypoints(pts, 2U, NULL, NULL, &plan))
    {
        return false;
    }
    plan.max_vel = parser->feed_rate;
    plan.max_acc = planner->config.max_acceleration;
    plan.max_jerk = planner->config.max_jerk;
    parser_apply_position(parser, target);
    return planner_enqueue_spline(planner, &plan);
}

static bool enqueue_arc(gcode_parser *parser,
                        planner_context *planner,
                        const gcode_modal_tokens *tokens,
                        bool clockwise,
                        const vec3_q16 *target)
{
    if (!(tokens->has_i || tokens->has_j || tokens->has_r))
    {
        return false;
    }
    float scale = unit_scale(parser);
    vec3_q16 start = parser->position;
    float sx = q16_to_float(start.x);
    float sy = q16_to_float(start.y);
    float ex = q16_to_float(target->x);
    float ey = q16_to_float(target->y);
    float cx;
    float cy;
    if (tokens->has_r)
    {
        float radius = (float)tokens->r * scale;
        float midx = (sx + ex) * 0.5f;
        float midy = (sy + ey) * 0.5f;
        float dx = ex - sx;
        float dy = ey - sy;
        float d2 = (dx * dx) + (dy * dy);
        if ((d2 == 0.0f) || (radius < sqrtf(d2) * 0.5f))
        {
            return false;
        }
        float term = (radius * radius) / (d2 * 0.25f) - 1.0f;
        if (term < 0.0f)
        {
            term = 0.0f;
        }
        float q = sqrtf(term);
        if (clockwise)
        {
            q = -q;
        }
        cx = midx - q * dy * 0.5f;
        cy = midy + q * dx * 0.5f;
    }
    else
    {
        cx = sx + (float)tokens->i * scale;
        cy = sy + (float)tokens->j * scale;
    }
    float start_ang = atan2f(sy - cy, sx - cx);
    float end_ang = atan2f(ey - cy, ex - cx);
    float sweep = end_ang - start_ang;
    if (clockwise && (sweep > 0.0f))
    {
        sweep -= 2.0f * (float)M_PI;
    }
    else if (!clockwise && (sweep < 0.0f))
    {
        sweep += 2.0f * (float)M_PI;
    }
    size_t segments = (size_t)(fabsf(sweep) / ((float)M_PI / 12.0f)) + 2U;
    if (segments < 3U)
    {
        segments = 3U;
    }
    if (segments > 64U)
    {
        segments = 64U;
    }
    vec3_q16 pts[64];
    pts[0] = start;
    for (size_t idx = 1U; idx < (segments - 1U); ++idx)
    {
        float t = (float)idx / (float)(segments - 1U);
        float ang = start_ang + sweep * t;
        float px = cx + cosf(ang) * (sx - cx);
        float py = cy + sinf(ang) * (sy - cy);
        pts[idx] = vec3_from_float(px, py, q16_to_float(start.z));
    }
    pts[segments - 1U] = *target;
    spl_plan_t plan;
    if (!spl_make_from_waypoints(pts, segments, NULL, NULL, &plan))
    {
        return false;
    }
    plan.max_vel = parser->feed_rate;
    plan.max_acc = planner->config.max_acceleration;
    plan.max_jerk = planner->config.max_jerk;
    parser_apply_position(parser, target);
    return planner_enqueue_spline(planner, &plan);
}

static bool enqueue_bezier(gcode_parser *parser,
                           planner_context *planner,
                           const gcode_modal_tokens *tokens,
                           const vec3_q16 *target,
                           bool nurbs)
{
    if (!parser->enable_splines)
    {
        return false;
    }
    float scale = unit_scale(parser);
    vec3_q16 pts[4];
    pts[0] = parser->position;
    vec3_q16 c1 = pts[0];
    vec3_q16 c2 = pts[0];
    if (tokens->has_i)
    {
        c1.x = q16_add(c1.x, q16_from_float((float)tokens->i * scale));
    }
    if (tokens->has_j)
    {
        c1.y = q16_add(c1.y, q16_from_float((float)tokens->j * scale));
    }
    if (tokens->has_k)
    {
        c1.z = q16_add(c1.z, q16_from_float((float)tokens->k * scale));
    }
    if (tokens->has_p)
    {
        c2.x = q16_add(c2.x, q16_from_float((float)tokens->p * scale));
    }
    if (tokens->has_q)
    {
        c2.y = q16_add(c2.y, q16_from_float((float)tokens->q * scale));
    }
    if (tokens->has_r)
    {
        c2.z = q16_add(c2.z, q16_from_float((float)tokens->r * scale));
    }
    pts[1] = c1;
    pts[2] = c2;
    pts[3] = *target;
    spl_plan_t plan;
    if (!spl_make_from_waypoints(pts, 4U, NULL, NULL, &plan))
    {
        return false;
    }
    plan.type = nurbs ? SPL_NURBS3 : SPL_BEZIER3;
    plan.max_vel = parser->feed_rate;
    plan.max_acc = planner->config.max_acceleration;
    plan.max_jerk = planner->config.max_jerk;
    parser_apply_position(parser, target);
    return planner_enqueue_spline(planner, &plan);
}

void gcode_parser_init(gcode_parser *parser, bool enable_splines)
{
    if (parser == NULL)
    {
        return;
    }
    parser->enable_splines = enable_splines;
    gcode_parser_reset(parser);
}

void gcode_parser_reset(gcode_parser *parser)
{
    if (parser == NULL)
    {
        return;
    }
    parser->absolute_mode = true;
    parser->metric_units = true;
    parser->feed_rate = q16_from_float(100.0f);
    parser->spindle_speed = q16_from_int(0);
    parser->position = vec3_from_float(0.0f, 0.0f, 0.0f);
    parser->last_report[0] = '\0';
}

static void handle_modal_updates(gcode_parser *parser, const gcode_modal_tokens *tokens)
{
    for (size_t idx = 0U; idx < tokens->g_count; ++idx)
    {
        double code = tokens->g_codes[idx];
        int major = (int)code;
        int minor = (int)((code - (double)major) * 10.0 + 0.5);
        switch (major)
        {
            case 20:
                parser->metric_units = false;
                set_report(parser, "Modal: inch units enabled");
                break;
            case 21:
                parser->metric_units = true;
                set_report(parser, "Modal: millimetre units enabled");
                break;
            case 90:
                parser->absolute_mode = true;
                set_report(parser, "Modal: absolute positioning");
                break;
            case 91:
                parser->absolute_mode = false;
                set_report(parser, "Modal: relative positioning");
                break;
            case 5:
                if (minor == 2)
                {
                    set_report(parser, "Preparing NURBS segment");
                }
                else if (minor == 3)
                {
                    set_report(parser, "Committing NURBS segment");
                }
                break;
            default:
                break;
        }
    }
    for (size_t idx = 0U; idx < tokens->m_count; ++idx)
    {
        switch (tokens->m_codes[idx])
        {
            case 3:
                set_report(parser, "Spindle forward");
                break;
            case 4:
                set_report(parser, "Spindle reverse");
                break;
            case 5:
                set_report(parser, "Spindle stop");
                break;
            case 17:
                set_report(parser, "Axes enabled");
                break;
            case 18:
                set_report(parser, "Axes disabled");
                break;
            case 112:
                set_report(parser, "Emergency stop requested");
                break;
            default:
                break;
        }
    }
    if (tokens->has_feed)
    {
        parser->feed_rate = q16_from_float((float)tokens->feed / 60.0f);
    }
    if (tokens->has_spindle)
    {
        parser->spindle_speed = q16_from_float((float)tokens->spindle);
    }
}

bool gcode_parse_line(gcode_parser *parser, planner_context *planner, const char *line)
{
    if ((parser == NULL) || (planner == NULL) || (line == NULL))
    {
        return false;
    }
    gcode_modal_tokens tokens;
    tokenise_line(line, &tokens);
    if ((tokens.g_count == 0U) && (tokens.m_count == 0U) && !tokens.has_x && !tokens.has_y &&
        !tokens.has_z && !tokens.has_feed && !tokens.has_spindle && !tokens.has_dwell)
    {
        return true;
    }
    handle_modal_updates(parser, &tokens);

    bool executed = false;
    for (size_t idx = 0U; idx < tokens.g_count; ++idx)
    {
        double code = tokens.g_codes[idx];
        int major = (int)code;
        int minor = (int)((code - (double)major) * 10.0 + 0.5);
        vec3_q16 target = make_target_position(parser, &tokens);
        switch (major)
        {
            case 0:
            case 1:
                if (enqueue_linear(parser, planner, &target))
                {
                    set_report(parser,
                               "Linear move to X%.3f Y%.3f Z%.3f feed %.2f",
                               q16_to_float(target.x),
                               q16_to_float(target.y),
                               q16_to_float(target.z),
                               q16_to_float(parser->feed_rate));
                    executed = true;
                }
                break;
            case 2:
            case 3:
                if (enqueue_arc(parser, planner, &tokens, (major == 2), &target))
                {
                    set_report(parser,
                               "Arc move %s to X%.3f Y%.3f Z%.3f",
                               (major == 2) ? "CW" : "CCW",
                               q16_to_float(target.x),
                               q16_to_float(target.y),
                               q16_to_float(target.z));
                    executed = true;
                }
                break;
            case 4:
                if (tokens.has_dwell)
                {
                    set_report(parser, "Dwell %.3f ms", tokens.dwell);
                    executed = true;
                }
                break;
            case 5:
                if ((minor == 0) && enqueue_bezier(parser, planner, &tokens, &target, false))
                {
                    set_report(parser, "Bezier spline to X%.3f Y%.3f Z%.3f",
                               q16_to_float(target.x),
                               q16_to_float(target.y),
                               q16_to_float(target.z));
                    executed = true;
                }
                else if ((minor == 2) && enqueue_bezier(parser, planner, &tokens, &target, true))
                {
                    set_report(parser, "NURBS-lite spline to X%.3f Y%.3f Z%.3f",
                               q16_to_float(target.x),
                               q16_to_float(target.y),
                               q16_to_float(target.z));
                    executed = true;
                }
                break;
            case 20:
            case 21:
            case 90:
            case 91:
                executed = true;
                break;
            case 64:
                set_report(parser, "S-curve blend request acknowledged");
                executed = true;
                break;
            default:
                break;
        }
    }
    if (!executed && (tokens.g_count == 0U))
    {
        executed = true;
    }
    return executed;
}

const vec3_q16 *gcode_parser_current_position(const gcode_parser *parser)
{
    return (parser != NULL) ? &parser->position : NULL;
}

const char *gcode_parser_last_report(const gcode_parser *parser)
{
    if (parser == NULL)
    {
        return "";
    }
    return parser->last_report;
}
