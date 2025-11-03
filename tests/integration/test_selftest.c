#include <stdbool.h>

#include "core/cnc.h"
#include "utils/q16.h"
#include "utils/vec3.h"

bool test_selftest_sequence(void)
{
    cnc_context cnc;
    if (!cnc_init(&cnc))
    {
        return false;
    }
    vec3_q16 circle = {q16_from_int(5), q16_from_int(0), q16_from_int(-320)};
    planner_queue_cartesian(&cnc.planner, circle, q16_from_int(50));
    int steps = 0;
    for (int i = 0; i < 100; ++i)
    {
        if (cnc_run_cycle(&cnc))
        {
            ++steps;
        }
    }
    cnc_shutdown(&cnc);
    return steps > 0;
}
