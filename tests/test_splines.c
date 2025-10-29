#include "test_suite.h"

#include <assert.h>

#include "../planner/splines/splines.h"

void test_splines(void)
{
    vec3_q16 pts[4] = {
        {0, 0, 0},
        {q16_from_int(10), 0, 0},
        {q16_from_int(10), q16_from_int(10), 0},
        {q16_from_int(20), q16_from_int(10), 0}};
    spl_plan_t plan;
    assert(spl_make_from_waypoints(pts, 4, NULL, NULL, &plan));
    vec3_q16 pos;
    vec3_q16 vel;
    assert(spl_sample_arc(&plan, q16_from_float(0.5f), &pos, &vel));
    time_scaled_traj_t traj;
    assert(spl_time_scale(&plan, q16_from_int(1), &traj));
    assert(traj.count > 0U);
}
