#ifndef ETHCAT_MASTER_H
#define ETHCAT_MASTER_H

#include <stdint.h>
#include <stdbool.h>
#include "board/config.h"
#include "utils/fixed.h"

typedef struct {
    uint16_t statusword;
    q16_16_t position_actual;
    q16_16_t velocity_actual;
    q16_16_t torque_actual;
    uint8_t mode_display;
    uint16_t emcy_code;
} ethcat_txpdo_t;

typedef struct {
    uint16_t controlword;
    q16_16_t target_position;
    q16_16_t target_velocity;
    q16_16_t target_torque;
    uint8_t mode_of_operation;
} ethcat_rxpdo_t;

typedef struct {
    uint32_t vendor_id;
    uint32_t product_code;
    uint16_t alias;
    ethcat_txpdo_t txpdo;
    ethcat_rxpdo_t rxpdo;
    bool present;
    bool operational;
} ethcat_slave_t;

typedef struct {
    ethcat_slave_t slaves[ECAT_MAX_SLAVES];
    uint32_t cycle_time_ns;
    int32_t dc_offset_ns;
    int32_t dc_drift_ppb;
    bool dc_synchronized;
    bool link_up;
} ethcat_master_t;

void ethcat_master_init(ethcat_master_t *master, const board_runtime_config_t *config);
bool ethcat_master_scan(ethcat_master_t *master);
bool ethcat_master_configure(ethcat_master_t *master);
void ethcat_master_sync0_handler(ethcat_master_t *master);
void ethcat_master_process(ethcat_master_t *master);
void ethcat_master_set_target(ethcat_master_t *master, int axis, const ethcat_rxpdo_t *rxpdo);
const ethcat_txpdo_t *ethcat_master_get_feedback(const ethcat_master_t *master, int axis);
bool ethcat_master_sdo_write(ethcat_master_t *master, int axis, uint16_t index, uint8_t subindex, uint32_t value);
bool ethcat_master_sdo_read(ethcat_master_t *master, int axis, uint16_t index, uint8_t subindex, uint32_t *value);
void ethcat_master_log_emergency(ethcat_master_t *master, int axis, uint16_t code);

#endif
