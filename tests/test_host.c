#include "test_suite.h"
#include "../core/main.h"
#include <assert.h>

void test_host(void)
{
    int rc = core_main();
    assert(rc == 0);
}
