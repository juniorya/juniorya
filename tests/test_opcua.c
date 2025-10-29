#include "test_suite.h"

#include <assert.h>

#include "../opcua/opcua.h"
#include "../core/cnc.h"

void test_opcua(void)
{
    cnc_context_t cnc;
    cnc_init(&cnc);
    opcua_server_t server;
    assert(opcua_server_init(&server, &cnc));
    opcua_server_poll(&server);
}
