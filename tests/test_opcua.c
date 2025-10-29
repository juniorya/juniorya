#include "test_suite.h"
#include "../opcua/server.h"
#include <assert.h>

void test_opcua(void)
{
    storage_params_t params;
    storage_defaults(&params);
    assert(opcua_server_start(&params));
    cnc_state_init();
    cnc_status_t status = cnc_state_status();
    opcua_server_publish(&status);
}
