#include "test_suite.h"

#include <assert.h>

#include "../planner/planner.h"

void test_planner(void)
{
    planner_limits_t limits = {q16_from_int(200), q16_from_int(1000), q16_from_int(5000)};
    planner_t planner;
    planner_init(&planner, &limits);
    vec3_q16 start = {0, 0, 0};
    vec3_q16 end = {q16_from_int(10), 0, 0};
    assert(planner_enqueue_linear(&planner, &start, &end));
    vec3_q16 point;
    assert(planner_step(&planner, q16_from_int(1), &point));
}
