/**
 * @file opcua.h
 * @brief OPC UA server interface.
 */

#ifndef OPCUA_H
#define OPCUA_H

#include <stdbool.h>

#include "utils/q16.h"

#ifdef __cplusplus
extern "C" {
#endif

struct cnc_context;

/** @brief OPC UA server context. */
typedef struct
{
    bool running;            /**< Server running flag. */
    q16_16 last_alignment;   /**< Cached alignment error from vision subsystem. */
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
 * @brief Publish latest alignment error into the OPC UA address space.
 * @param server Server instance.
 * @param error Alignment error magnitude (Q16.16).
 */
void opcua_server_publish_alignment(opcua_server *server, q16_16 error);

/**
 * @brief Shutdown OPC UA server.
 * @param server Server instance.
 */
void opcua_server_shutdown(opcua_server *server);

#ifdef __cplusplus
}
#endif

#endif
