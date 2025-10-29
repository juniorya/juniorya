#include "test_suite.h"

#include <assert.h>

#include "../core/cnc.h"
#include "../planner/planner.h"
#include "../motion/motion.h"
#include "../ethcat/ethcat.h"

void test_host(void)
{
    cnc_context_t cnc;
    cnc_init(&cnc);
    planner_limits_t limits = {q16_from_int(200), q16_from_int(1000), q16_from_int(5000)};
    planner_t planner;
    planner_init(&planner, &limits);
    vec3_q16 start = {0, 0, 0};
    vec3_q16 end = {q16_from_int(5), 0, 0};
    assert(planner_enqueue_linear(&planner, &start, &end));
    ethcat_master_t master;
    assert(ethcat_master_init(&master, 1000000U));
    motion_controller_t motion;
    motion_init(&motion, &planner, &master);
    motion_sync0_tick(&motion, q16_from_int(1));
}
