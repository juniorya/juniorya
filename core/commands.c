/**
 * @file commands.c
 * @brief Text command handling for CNC console.
 */

#include "commands.h"

#include <stdio.h>
#include <string.h>

void commands_process_line(cnc_context *cnc, const char *line)
{
    if (strncmp(line, "$H", 2) == 0)
    {
        cnc->state = CNC_STATE_RUNNING;
    }
    else if (strncmp(line, "$CAL", 4) == 0)
    {
        printf("ok\n");
    }
    else if (strncmp(line, "$SELFTEST", 9) == 0)
    {
        printf("selftest-start\n");
    }
    else
    {
        printf("error:1\n");
    }
}
