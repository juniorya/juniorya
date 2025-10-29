/**
 * @file opcua.h
 * @brief OPC UA server interface.
 */

#ifndef OPCUA_H
#define OPCUA_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

struct cnc_context;

/** @brief OPC UA server context. */
typedef struct
{
    bool running; /**< Server running flag. */
} opcua_server;

/**
 * @brief Initialise OPC UA server.
 * @param server Server instance.
 * @param cnc CNC context for data exposure.
 * @return True on success.
 */
bool opcua_server_init(opcua_server *server, struct cnc_context *cnc);

/**
 * @brief Run OPC UA processing step.
 * @param server Server instance.
 */
void opcua_server_step(opcua_server *server);

/**
 * @brief Shutdown OPC UA server.
 * @param server Server instance.
 */
void opcua_server_shutdown(opcua_server *server);

#ifdef __cplusplus
}
#endif

#endif
