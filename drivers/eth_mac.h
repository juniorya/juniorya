#ifndef DRIVERS_ETH_MAC_H
#define DRIVERS_ETH_MAC_H

#include <stdint.h>
#include <stdbool.h>

typedef void (*eth_sync_callback_t)(void *user_data);

typedef struct {
    uint8_t mac_address[6];
    uint32_t phy_address;
} eth_mac_config_t;

void eth_mac_init(const eth_mac_config_t *config, eth_sync_callback_t sync0_cb, void *user_data);
void eth_mac_set_sync_callback(eth_sync_callback_t sync0_cb, void *user_data);
void eth_mac_poll(void);
bool eth_mac_send_frame(const uint8_t *data, uint16_t length);
int  eth_mac_receive_frame(uint8_t *data, uint16_t max_length);
uint64_t eth_mac_get_time_ns(void);
void eth_mac_adjust_time(int32_t ns_offset);

#endif
