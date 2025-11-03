#ifndef OPCUA_H
#define OPCUA_H

#include <stdbool.h>

#include "utils/q16.h"
#include "motion/motion.h"

/** \brief OPC UA server context (mock implementation). */
typedef struct
{
    q16_16 last_position[3]; /**< Last published cartesian position. */
    bool running;            /**< Server state. */
} opcua_server;

bool opcua_server_init(opcua_server *server);

void opcua_server_shutdown(opcua_server *server);

void opcua_server_publish_pose(opcua_server *server, vec3_q16 position);

#endif
