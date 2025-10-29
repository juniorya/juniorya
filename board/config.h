#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H

#include <stdint.h>
#include "kinematics/delta.h"

#define CONTROL_PERIOD_US 1000U
#define CONTROL_PERIOD_FAST_US 500U

#define ECAT_MAX_SLAVES 3

typedef struct {
    uint32_t vendor_id;
    uint32_t product_code;
    uint16_t alias;
} ecat_slave_descriptor_t;

typedef struct {
    delta_cfg_t delta;
    q16_16_t axis_velocity_limit;
    q16_16_t axis_acceleration_limit;
    q16_16_t axis_jerk_limit;
    uint8_t default_mode_of_operation;
    ecat_slave_descriptor_t slaves[ECAT_MAX_SLAVES];
} board_runtime_config_t;

extern board_runtime_config_t g_board_config;

void board_clock_init(void);
void board_gpio_init(void);
void board_emac_init(void);
void board_console_init(void);
void board_load_configuration(void);

#endif
