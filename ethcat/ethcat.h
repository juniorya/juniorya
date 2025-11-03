#ifndef ETHCAT_H
#define ETHCAT_H

#include <stdbool.h>

#include "utils/q16.h"

/** \brief EtherCAT master configuration. */
typedef struct
{
    q16_16 sync0_rate_hz; /**< Sync0 frequency. */
} ethcat_master_config;

/** \brief EtherCAT master state. */
typedef struct
{
    q16_16 last_setpoint[3]; /**< Last CSP target. */
    ethcat_master_config cfg;/**< Copy of configuration. */
    bool operational;        /**< Link status. */
} ethcat_master;

void ethcat_master_default_config(ethcat_master_config *cfg);

bool ethcat_master_init(ethcat_master *master, const ethcat_master_config *cfg);

void ethcat_master_shutdown(ethcat_master *master);

bool ethcat_master_send_setpoints(ethcat_master *master, const q16_16 joints[3]);

void ethcat_master_poll(ethcat_master *master);

#endif
