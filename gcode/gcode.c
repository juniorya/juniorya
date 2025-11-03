#include "gcode/gcode.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

static bool parse_word(const char **cursor, char word, q16_16 *out_value)
{
    const char *ptr = *cursor;
    if ((ptr[0] != word) && (ptr[0] != (char)toupper((int)word)))
    {
        return false;
    }
    ptr++;
    char *endptr = NULL;
    float value = strtof(ptr, &endptr);
    if (endptr == ptr)
    {
        return false;
    }
    *out_value = q16_from_float(value);
    *cursor = endptr;
    return true;
}

gcode_result gcode_parse_line(const char *line, gcode_block *out)
{
    if ((line == NULL) || (out == NULL))
    {
        return GCODE_INVALID;
    }
    memset(out, 0, sizeof(*out));
    const char *ptr = line;
    bool content = false;
    while (*ptr != '\0')
    {
        if ((*ptr == ';') || (*ptr == '('))
        {
            break;
        }
        if (isspace((unsigned char)*ptr))
        {
            ++ptr;
            continue;
        }
        if ((*ptr == 'G') || (*ptr == 'g'))
        {
            ++ptr;
            char *endptr = NULL;
            long value = strtol(ptr, &endptr, 10);
            if (endptr == ptr)
            {
                return GCODE_INVALID;
            }
            out->has_g = true;
            out->g_code = (int)value;
            ptr = endptr;
            content = true;
            continue;
        }
        if ((*ptr == 'M') || (*ptr == 'm'))
        {
            ++ptr;
            char *endptr = NULL;
            long value = strtol(ptr, &endptr, 10);
            if (endptr == ptr)
            {
                return GCODE_INVALID;
            }
            out->has_m = true;
            out->m_code = (int)value;
            ptr = endptr;
            content = true;
            continue;
        }
        q16_16 value = 0;
        if (parse_word(&ptr, 'X', &value))
        {
            out->has_x = true;
            out->x = value;
            content = true;
            continue;
        }
        if (parse_word(&ptr, 'Y', &value))
        {
            out->has_y = true;
            out->y = value;
            content = true;
            continue;
        }
        if (parse_word(&ptr, 'Z', &value))
        {
            out->has_z = true;
            out->z = value;
            content = true;
            continue;
        }
        if (parse_word(&ptr, 'I', &value))
        {
            out->has_i = true;
            out->i = value;
            content = true;
            continue;
        }
        if (parse_word(&ptr, 'J', &value))
        {
            out->has_j = true;
            out->j = value;
            content = true;
            continue;
        }
        if (parse_word(&ptr, 'F', &value))
        {
            out->has_feed = true;
            out->feed = value;
            content = true;
            continue;
        }
        return GCODE_INVALID;
    }
    return content ? GCODE_OK : GCODE_EMPTY;
}
