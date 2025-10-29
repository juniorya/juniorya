/**
 * @file ethcat.c
 * @brief Simulated EtherCAT master implementation.
 */
#include "ethcat.h"

#include <string.h>

bool ethcat_master_init(ethcat_master_t *master, uint32_t cycle_time_ns)
{
    memset(master, 0, sizeof(*master));
    master->cycle_time_ns = cycle_time_ns;
    master->link_up = true;
    return true;
}

void ethcat_master_process(ethcat_master_t *master, const vec3_q16 *cartesian)
{
    (void)cartesian;
    for (size_t i = 0U; i < 3U; ++i)
    {
        master->feedback[i].position = master->setpoints[i].target_position;
        master->feedback[i].velocity = master->setpoints[i].target_velocity;
        master->feedback[i].following_error = 0;
        master->feedback[i].status_word = 0x0237U;
    }
}

void ethcat_master_set_axis(ethcat_master_t *master,
                            size_t axis,
                            const ethcat_axis_setpoint_t *setpoint)
{
    if (axis < 3U)
    {
        master->setpoints[axis] = *setpoint;
    }
}

const ethcat_axis_feedback_t *ethcat_master_get_feedback(const ethcat_master_t *master,
                                                         size_t axis)
{
    if (axis < 3U)
    {
        return &master->feedback[axis];
    }
    return NULL;
}
