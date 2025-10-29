#include "test_suite.h"
#include "../planner/planner.h"
#include <assert.h>

void test_planner(void)
{
    planner_t planner;
    planner_config_t cfg = {
        .max_vel = fixed_from_double(500.0),
        .max_acc = fixed_from_double(1000.0),
        .max_jerk = fixed_from_double(5000.0)
    };
    planner_init(&planner, &cfg);
    delta_pose_t pose = {.xyz = {fixed_from_double(10.0), 0, 0}};
    assert(planner_enqueue_line(&planner, &pose, fixed_from_double(100.0)));
    trajectory_segment_t seg;
    assert(planner_next_segment(&planner, &seg));
}
