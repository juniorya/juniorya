#include "test_suite.h"

#include <assert.h>

#include "../ethcat/ethcat.h"

void test_ethcat(void)
{
    ethcat_master_t master;
    assert(ethcat_master_init(&master, 1000000U));
    vec3_q16 cart = {0};
    ethcat_axis_setpoint_t setpoint = {q16_from_int(10), q16_from_int(1), 0x000F};
    ethcat_master_set_axis(&master, 0U, &setpoint);
    ethcat_master_process(&master, &cart);
    const ethcat_axis_feedback_t *fb = ethcat_master_get_feedback(&master, 0U);
    assert(fb != NULL);
    assert(fb->position == q16_from_int(10));
}
