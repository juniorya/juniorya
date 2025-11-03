#ifndef GCODE_H
#define GCODE_H

#include <stdbool.h>
#include <stddef.h>

#include "utils/q16.h"

/** \brief Parsed G-code block. */
typedef struct
{
    bool has_g;        /**< Whether a G word is present. */
    int g_code;        /**< Integer part of the G word. */
    bool has_m;        /**< Whether an M word is present. */
    int m_code;        /**< Integer part of the M word. */
    q16_16 x;          /**< X coordinate (if has_x). */
    q16_16 y;          /**< Y coordinate (if has_y). */
    q16_16 z;          /**< Z coordinate (if has_z). */
    bool has_x;        /**< Flag for X word. */
    bool has_y;        /**< Flag for Y word. */
    bool has_z;        /**< Flag for Z word. */
    q16_16 i;          /**< Arc centre X offset. */
    q16_16 j;          /**< Arc centre Y offset. */
    bool has_i;        /**< Flag for I word. */
    bool has_j;        /**< Flag for J word. */
    q16_16 feed;       /**< Feed rate. */
    bool has_feed;     /**< Feed rate specified. */
} gcode_block;

/** \brief Result of parsing a block. */
typedef enum
{
    GCODE_OK,       /**< Line parsed successfully. */
    GCODE_EMPTY,    /**< Line was empty or comment only. */
    GCODE_INVALID   /**< Syntax error. */
} gcode_result;

/** \brief Parse single G-code line. */
gcode_result gcode_parse_line(const char *line, gcode_block *out);

#endif
