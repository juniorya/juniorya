#include "test_suite.h"

#include <assert.h>

#include "../gcode/gcode.h"
#include "../planner/planner.h"

void test_console(void)
{
    planner_limits_t limits = {q16_from_int(200), q16_from_int(1000), q16_from_int(5000)};
    planner_t planner;
    planner_init(&planner, &limits);
    assert(gcode_parse_line(&planner, "G1 X10 Y0 Z-5"));
}
