/**
 * @file gcode.c
 * @brief Simple G-code parser implementation.
 */
#include "gcode.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "utils/q16.h"

bool gcode_parse_line(planner_t *planner, const char *line)
{
    vec3_q16 start = {0};
    vec3_q16 end = {0};
    char command = '\0';
    int code = 0;

    const char *ptr = line;
    while (*ptr != '\0')
    {
        if (isspace((int)*ptr))
        {
            ++ptr;
            continue;
        }
        if (isalpha((int)*ptr))
        {
            command = (char)toupper((int)*ptr);
            ++ptr;
            code = atoi(ptr);
            while (isdigit((int)*ptr))
            {
                ++ptr;
            }
        }
        else if (*ptr == 'X' || *ptr == 'Y' || *ptr == 'Z')
        {
            char axis = (char)*ptr++;
            float value = strtof(ptr, (char **)&ptr);
            q16_16 qval = q16_from_float(value);
            if (axis == 'X')
            {
                end.x = qval;
            }
            else if (axis == 'Y')
            {
                end.y = qval;
            }
            else
            {
                end.z = qval;
            }
        }
        else
        {
            ++ptr;
        }
    }

    if (command == 'G' && (code == 0 || code == 1))
    {
        return planner_enqueue_linear(planner, &start, &end);
    }

    return true;
}
