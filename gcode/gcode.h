/**
 * @file gcode.h
 * @brief Lightweight G-code parser.
 */

#ifndef GCODE_H
#define GCODE_H

#include <stdbool.h>

#include "planner/planner.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Parser instance. */
typedef struct
{
    bool enable_splines; /**< Enable G5/G5.2 commands. */
} gcode_parser;

/**
 * @brief Initialise parser.
 * @param parser Parser instance.
 * @param enable_splines Enable advanced spline commands.
 */
void gcode_parser_init(gcode_parser *parser, bool enable_splines);

/**
 * @brief Parse single G-code line and enqueue trajectory.
 * @param parser Parser instance.
 * @param planner Planner to receive segments.
 * @param line Null-terminated line.
 * @return True if line was accepted.
 */
bool gcode_parse_line(gcode_parser *parser, planner_context *planner, const char *line);

#ifdef __cplusplus
}
#endif

#endif
