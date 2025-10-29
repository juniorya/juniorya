/**
 * @file opcua.h
 * @brief OPC UA server shim using open62541-compatible interface.
 */
#ifndef OPCUA_OPCUA_H
#define OPCUA_OPCUA_H

#include <stdbool.h>
#include "core/cnc.h"

/**
 * @brief OPC UA server context.
 */
typedef struct
{
    cnc_context_t *cnc;
    bool running;
} opcua_server_t;

/**
 * @brief Initialize OPC UA server.
 *
 * @param server Server context.
 * @param cnc CNC context to expose.
 * @return true on success.
 */
bool opcua_server_init(opcua_server_t *server, cnc_context_t *cnc);

/**
 * @brief Process network events.
 *
 * @param server Server context.
 */
void opcua_server_poll(opcua_server_t *server);

#endif /* OPCUA_OPCUA_H */
