#include "test_suite.h"
#include "../ethcat/master.h"
#include <assert.h>

void test_ethcat(void)
{
    assert(ethcat_master_init("ec0"));
    ethcat_master_cycle();
}
