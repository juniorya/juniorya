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
    server->last_alignment = 0;
    log_printf(LOG_INFO, "OPC UA server initialised");
    return true;
}

void opcua_server_step(opcua_server *server)
{
    if (!server->running)
    {
        return;
    }

    if (server->last_alignment != 0)
    {
        const float error_mm = q16_to_float(server->last_alignment);
        log_printf(LOG_INFO, "OPC UA alignment error %.3f mm", (double)error_mm);
    }
}

void opcua_server_publish_alignment(opcua_server *server, q16_16 error)
{
    if (server == NULL)
    {
        return;
    }

    server->last_alignment = error;
}

void opcua_server_shutdown(opcua_server *server)
{
    server->running = false;
    log_printf(LOG_INFO, "OPC UA server shutdown");
}
