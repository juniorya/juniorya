/**
 * @file commands.h
 * @brief Interfaces for handling console commands and G-code execution.
 */

#ifndef COMMANDS_H
#define COMMANDS_H

#include "core/cnc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Process inbound command line.
 * @param cnc CNC context.
 * @param line Null-terminated command string.
 */
void commands_process_line(cnc_context *cnc, const char *line);

#ifdef __cplusplus
}
#endif

#endif
