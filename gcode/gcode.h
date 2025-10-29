/**
 * @file gcode.h
 * @brief Lightweight G-code parser supporting core commands.
 */
#ifndef GCODE_GCODE_H
#define GCODE_GCODE_H

#include <stdbool.h>
#include "planner/planner.h"

/**
 * @brief Parse a line of G-code.
 *
 * @param planner Planner instance to enqueue resulting moves.
 * @param line Null-terminated G-code line.
 * @return true on success.
 */
bool gcode_parse_line(planner_t *planner, const char *line);

#endif /* GCODE_GCODE_H */
