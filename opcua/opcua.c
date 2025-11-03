#include "opcua/opcua.h"

bool opcua_server_init(opcua_server *server)
{
    if (server == NULL)
    {
        return false;
    }
    server->last_position[0] = 0;
    server->last_position[1] = 0;
    server->last_position[2] = 0;
    server->running = true;
    return true;
}

void opcua_server_shutdown(opcua_server *server)
{
    if (server != NULL)
    {
        server->running = false;
    }
}

void opcua_server_publish_pose(opcua_server *server, vec3_q16 position)
{
    if ((server == NULL) || !server->running)
    {
        return;
    }
    server->last_position[0] = position.x;
    server->last_position[1] = position.y;
    server->last_position[2] = position.z;
}
