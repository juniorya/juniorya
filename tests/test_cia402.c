#include "test_suite.h"

#include <assert.h>

#include "../cia402/cia402.h"

void test_cia402(void)
{
    cia402_axis_t axis;
    cia402_init_axis(&axis, CIA402_MODE_CSP);
    cia402_push_setpoints(&axis, q16_from_int(10), q16_from_int(1));
    assert(axis.state == CIA402_STATE_OPERATION_ENABLED);
    cia402_handle_fault(&axis);
    assert(axis.state == CIA402_STATE_SWITCH_ON_DISABLED);
}
