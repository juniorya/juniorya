/**
 * @file gcode.h
 * @brief Lightweight G-code parser.
 */

#ifndef GCODE_H
#define GCODE_H

#include <stdbool.h>
#include <stddef.h>

#include "planner/planner.h"
#include "utils/vec3.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Parser instance. */
typedef struct
{
    bool enable_splines;          /**< Enable G5/G5.2 spline commands. */
    bool absolute_mode;           /**< True when operating in absolute coordinates (G90). */
    bool metric_units;            /**< True for millimetres (G21); false for inches (G20). */
    q16_16 feed_rate;             /**< Current feed rate (mm/min or converted inches/min). */
    q16_16 spindle_speed;         /**< Current spindle speed in RPM. */
    vec3_q16 position;            /**< Current tool position. */
    char last_report[256];        /**< Last debug message emitted by the parser. */
} gcode_parser;

/**
 * @brief Initialise parser.
 * @param parser Parser instance.
 * @param enable_splines Enable advanced spline commands.
 */
void gcode_parser_init(gcode_parser *parser, bool enable_splines);

/**
 * @brief Reset runtime state such as modal parameters and position.
 * @param parser Parser instance.
 */
void gcode_parser_reset(gcode_parser *parser);

/**
 * @brief Parse single G-code line and enqueue trajectory.
 * @param parser Parser instance.
 * @param planner Planner to receive segments.
 * @param line Null-terminated line.
 * @return True if line was accepted.
 */
bool gcode_parse_line(gcode_parser *parser, planner_context *planner, const char *line);

/**
 * @brief Obtain current parser position after processing commands.
 * @param parser Parser instance.
 * @return Pointer to internal position vector.
 */
const vec3_q16 *gcode_parser_current_position(const gcode_parser *parser);

/**
 * @brief Retrieve last debug report emitted by parser.
 * @param parser Parser instance.
 * @return Null-terminated diagnostic string.
 */
const char *gcode_parser_last_report(const gcode_parser *parser);

#ifdef __cplusplus
}
#endif

#endif
