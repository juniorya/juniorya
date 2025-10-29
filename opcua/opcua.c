/**
 * @file opcua.c
 * @brief Stub OPC UA server.
 */
#include "opcua.h"

bool opcua_server_init(opcua_server_t *server, cnc_context_t *cnc)
{
    server->cnc = cnc;
    server->running = true;
    return true;
}

void opcua_server_poll(opcua_server_t *server)
{
    (void)server;
}
