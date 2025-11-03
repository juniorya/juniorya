#include "dds/dds.h"
#include "opcua/opcua.h"
#include "utils/q16.h"
#include "utils/vec3.h"

#include <stdio.h>

int main(void)
{
    dds_qos_t qos = {DDS_RELIABILITY_RELIABLE, 10U};
    dds_init(0U, &qos);
    dds_writer *writer = dds_writer_create("cnc.telemetry", sizeof(q16_16) * 3U, &qos);
    if (writer == NULL)
    {
        return 1;
    }
    opcua_server server;
    opcua_server_init(&server);
    vec3_q16 pose = {q16_from_int(1), q16_from_int(2), q16_from_int(3)};
    opcua_server_publish_pose(&server, pose);
    q16_16 payload[3] = {pose.x, pose.y, pose.z};
    dds_write(writer, payload, sizeof(payload));
    printf("Bridge exported telemetry sample.\n");
    opcua_server_shutdown(&server);
    dds_shutdown();
    return 0;
}
