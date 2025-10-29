#include "test_suite.h"
#include "../planner/splines/splines.h"
#include <assert.h>

void test_splines(void)
{
    vec3_q16 pts[2];
    pts[0].v[0] = pts[0].v[1] = pts[0].v[2] = 0;
    pts[1].v[0] = pts[1].v[1] = pts[1].v[2] = fixed_from_double(10.0);
    spl_plan_t plan;
    assert(spl_make_from_waypoints(pts, 2, NULL, NULL, &plan));
    vec3_q16 pos;
    vec3_q16 vel;
    assert(spl_sample_arc(&plan, fixed_from_double(0.5), &pos, &vel));
    assert(pos.v[0] != 0);
}
