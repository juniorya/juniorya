#include <stdbool.h>

#include "planner/planner.h"
#include "utils/q16.h"
#include "utils/vec3.h"

bool test_planner_basic(void)
{
    planner_config cfg;
    planner_default_config(&cfg);
    planner_context ctx;
    if (!planner_init(&ctx, &cfg))
    {
        return false;
    }
    vec3_q16 target = {q16_from_int(10), 0, q16_from_int(-200)};
    if (!planner_queue_cartesian(&ctx, target, q16_from_int(100)))
    {
        return false;
    }
    vec3_q16 pos;
    vec3_q16 vel;
    bool any = false;
    for (int i = 0; i < 10; ++i)
    {
        if (planner_next_sample(&ctx, &pos, &vel))
        {
            any = true;
        }
    }
    return any;
}
