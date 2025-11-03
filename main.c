#include "core/cnc.h"
#include "gcode/gcode.h"
#include "osal/osal.h"
#include "utils/log.h"
#include "utils/q16.h"

#include <stdio.h>

int main(void)
{
    if (!osal_init())
    {
        fprintf(stderr, "OSAL init failed\n");
        return 1;
    }

    cnc_context cnc;
    if (!cnc_init(&cnc))
    {
        fprintf(stderr, "CNC init failed\n");
        return 1;
    }

    vec3_q16 target = {q16_from_int(10), q16_from_int(0), q16_from_int(-300)};
    planner_queue_cartesian(&cnc.planner, target, q16_from_int(100));

    for (int i = 0; i < 1000; ++i)
    {
        if (!cnc_run_cycle(&cnc))
        {
            break;
        }
        osal_sleep_ms(1U);
    }

    cnc_shutdown(&cnc);
    osal_shutdown();
    return 0;
}
