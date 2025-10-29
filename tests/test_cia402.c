#include "test_suite.h"
#include "../cia402/cia402.h"
#include <assert.h>

void test_cia402(void)
{
    cia402_axis_t axis;
    cia402_axis_init(&axis, CIA402_MODE_CSP);
    cia402_axis_set_target(&axis, fixed_from_double(5.0), fixed_from_double(100.0));
    cia402_axis_tick(&axis);
    assert(axis.actual_pos != 0);
}
