#include "master.h"
#include "drivers/eth_mac.h"
#include "utils/timer.h"
#include <string.h>

#define SDO_CACHE_MAX 16

typedef struct {
    uint16_t index;
    uint8_t subindex;
    uint32_t value;
    bool valid;
} sdo_cache_entry_t;

static board_runtime_config_t s_config;
static sdo_cache_entry_t s_sdo_cache[ECAT_MAX_SLAVES][SDO_CACHE_MAX];

void ethcat_master_init(ethcat_master_t *master, const board_runtime_config_t *config)
{
    memset(master, 0, sizeof(*master));
    s_config = *config;
    master->cycle_time_ns = 1000000U;
    master->dc_offset_ns = 0;
    master->dc_drift_ppb = 0;
    master->dc_synchronized = false;
    master->link_up = false;
    for (int axis = 0; axis < ECAT_MAX_SLAVES; ++axis) {
        master->slaves[axis].vendor_id = config->slaves[axis].vendor_id;
        master->slaves[axis].product_code = config->slaves[axis].product_code;
        master->slaves[axis].alias = config->slaves[axis].alias;
        master->slaves[axis].present = false;
        master->slaves[axis].operational = false;
        master->slaves[axis].rxpdo.mode_of_operation = config->default_mode_of_operation;
        master->slaves[axis].rxpdo.controlword = 0;
        master->slaves[axis].rxpdo.target_position = 0;
        master->slaves[axis].rxpdo.target_velocity = 0;
        master->slaves[axis].rxpdo.target_torque = 0;
        master->slaves[axis].txpdo.statusword = 0x0000U;
        master->slaves[axis].txpdo.position_actual = 0;
        master->slaves[axis].txpdo.velocity_actual = 0;
        master->slaves[axis].txpdo.torque_actual = 0;
        master->slaves[axis].txpdo.mode_display = config->default_mode_of_operation;
        master->slaves[axis].txpdo.emcy_code = 0x0000U;
        for (int entry = 0; entry < SDO_CACHE_MAX; ++entry) {
            s_sdo_cache[axis][entry].valid = false;
        }
    }
}

static void sdo_cache_store(int axis, uint16_t index, uint8_t subindex, uint32_t value)
{
    for (int i = 0; i < SDO_CACHE_MAX; ++i) {
        if (s_sdo_cache[axis][i].valid && s_sdo_cache[axis][i].index == index && s_sdo_cache[axis][i].subindex == subindex) {
            s_sdo_cache[axis][i].value = value;
            return;
        }
    }
    for (int i = 0; i < SDO_CACHE_MAX; ++i) {
        if (!s_sdo_cache[axis][i].valid) {
            s_sdo_cache[axis][i].valid = true;
            s_sdo_cache[axis][i].index = index;
            s_sdo_cache[axis][i].subindex = subindex;
            s_sdo_cache[axis][i].value = value;
            return;
        }
    }
    s_sdo_cache[axis][0].index = index;
    s_sdo_cache[axis][0].subindex = subindex;
    s_sdo_cache[axis][0].value = value;
    s_sdo_cache[axis][0].valid = true;
}

static bool sdo_cache_lookup(int axis, uint16_t index, uint8_t subindex, uint32_t *value)
{
    for (int i = 0; i < SDO_CACHE_MAX; ++i) {
        if (s_sdo_cache[axis][i].valid && s_sdo_cache[axis][i].index == index && s_sdo_cache[axis][i].subindex == subindex) {
            *value = s_sdo_cache[axis][i].value;
            return true;
        }
    }
    return false;
}

bool ethcat_master_scan(ethcat_master_t *master)
{
    master->link_up = true;
    for (int axis = 0; axis < ECAT_MAX_SLAVES; ++axis) {
        master->slaves[axis].present = true;
        master->slaves[axis].operational = false;
    }
    return true;
}

static void configure_default_sdos(ethcat_master_t *master)
{
    for (int axis = 0; axis < ECAT_MAX_SLAVES; ++axis) {
        ethcat_master_sdo_write(master, axis, 0x6081U, 0x00U, (uint32_t)s_config.axis_velocity_limit);
        ethcat_master_sdo_write(master, axis, 0x6083U, 0x00U, (uint32_t)s_config.axis_acceleration_limit);
        ethcat_master_sdo_write(master, axis, 0x607F, 0x00U, (uint32_t)s_config.axis_jerk_limit);
    }
}

bool ethcat_master_configure(ethcat_master_t *master)
{
    if (!master->link_up) {
        return false;
    }
    configure_default_sdos(master);
    master->dc_synchronized = true;
    master->dc_offset_ns = 0;
    master->dc_drift_ppb = 0;
    for (int axis = 0; axis < ECAT_MAX_SLAVES; ++axis) {
        master->slaves[axis].operational = true;
        master->slaves[axis].txpdo.statusword = 0x1234U;
    }
    return true;
}

void ethcat_master_sync0_handler(ethcat_master_t *master)
{
    if (!master->dc_synchronized) {
        return;
    }
    uint64_t dc_time = eth_mac_get_time_ns();
    int32_t error = (int32_t)(dc_time % master->cycle_time_ns);
    master->dc_offset_ns = error;
    if (error > (int32_t)(master->cycle_time_ns / 2U)) {
        eth_mac_adjust_time(-(int32_t)(error / 4));
    } else if (error < -(int32_t)(master->cycle_time_ns / 2U)) {
        eth_mac_adjust_time(-(int32_t)(error / 4));
    }
}

void ethcat_master_process(ethcat_master_t *master)
{
    (void)master;
    eth_mac_poll();
}

void ethcat_master_set_target(ethcat_master_t *master, int axis, const ethcat_rxpdo_t *rxpdo)
{
    if (axis < 0 || axis >= ECAT_MAX_SLAVES) {
        return;
    }
    master->slaves[axis].rxpdo = *rxpdo;
    master->slaves[axis].txpdo.mode_display = rxpdo->mode_of_operation;
    master->slaves[axis].txpdo.position_actual = rxpdo->target_position;
    master->slaves[axis].txpdo.velocity_actual = rxpdo->target_velocity;
    master->slaves[axis].txpdo.torque_actual = rxpdo->target_torque;
    master->slaves[axis].txpdo.statusword = 0x1437U;
}

const ethcat_txpdo_t *ethcat_master_get_feedback(const ethcat_master_t *master, int axis)
{
    if (axis < 0 || axis >= ECAT_MAX_SLAVES) {
        return NULL;
    }
    return &master->slaves[axis].txpdo;
}

bool ethcat_master_sdo_write(ethcat_master_t *master, int axis, uint16_t index, uint8_t subindex, uint32_t value)
{
    (void)master;
    if (axis < 0 || axis >= ECAT_MAX_SLAVES) {
        return false;
    }
    sdo_cache_store(axis, index, subindex, value);
    return true;
}

bool ethcat_master_sdo_read(ethcat_master_t *master, int axis, uint16_t index, uint8_t subindex, uint32_t *value)
{
    (void)master;
    if (axis < 0 || axis >= ECAT_MAX_SLAVES) {
        return false;
    }
    if (sdo_cache_lookup(axis, index, subindex, value)) {
        return true;
    }
    *value = 0U;
    return false;
}

void ethcat_master_log_emergency(ethcat_master_t *master, int axis, uint16_t code)
{
    if (axis < 0 || axis >= ECAT_MAX_SLAVES) {
        return;
    }
    master->slaves[axis].txpdo.emcy_code = code;
    master->slaves[axis].txpdo.statusword |= 0x0008U;
}
