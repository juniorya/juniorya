/**
 * @file opcua.c
 * @brief OPC UA server stub implementation.
 */

#include "opcua.h"

#include "core/cnc.h"
#include "log.h"

bool opcua_server_init(opcua_server *server, cnc_context *cnc)
{
    (void)cnc;
    server->running = true;
    log_printf(LOG_INFO, "OPC UA server initialised");
    return true;
}

void opcua_server_step(opcua_server *server)
{
    if (!server->running)
    {
        return;
    }
}

void opcua_server_shutdown(opcua_server *server)
{
    server->running = false;
    log_printf(LOG_INFO, "OPC UA server shutdown");
}
