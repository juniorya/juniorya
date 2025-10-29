#include "eth_mac.h"
#include "utils/timer.h"
#include <string.h>

#define ETH_MAC_TX_QUEUE 8
#define ETH_MAC_MAX_FRAME 1518

static struct {
    eth_sync_callback_t sync_cb;
    void *sync_user;
    uint8_t tx_queue[ETH_MAC_TX_QUEUE][ETH_MAC_MAX_FRAME];
    uint16_t tx_length[ETH_MAC_TX_QUEUE];
    uint8_t tx_head;
    uint8_t tx_tail;
    uint8_t rx_queue[ETH_MAC_TX_QUEUE][ETH_MAC_MAX_FRAME];
    uint16_t rx_length[ETH_MAC_TX_QUEUE];
    uint8_t rx_head;
    uint8_t rx_tail;
    uint64_t dc_time_ns;
} s_mac;

void eth_mac_init(const eth_mac_config_t *config, eth_sync_callback_t sync0_cb, void *user_data)
{
    (void)config;
    s_mac.sync_cb = sync0_cb;
    s_mac.sync_user = user_data;
    s_mac.tx_head = s_mac.tx_tail = 0U;
    s_mac.rx_head = s_mac.rx_tail = 0U;
    s_mac.dc_time_ns = 0ULL;
}

static bool queue_push(uint8_t queue[ETH_MAC_TX_QUEUE][ETH_MAC_MAX_FRAME],
                       uint16_t lengths[ETH_MAC_TX_QUEUE],
                       uint8_t *head,
                       uint8_t tail,
                       const uint8_t *data,
                       uint16_t length)
{
    uint8_t next = (uint8_t)((*head + 1U) % ETH_MAC_TX_QUEUE);
    if (next == tail) {
        return false;
    }
    memcpy(queue[*head], data, length);
    lengths[*head] = length;
    *head = next;
    return true;
}

static int queue_pop(uint8_t queue[ETH_MAC_TX_QUEUE][ETH_MAC_MAX_FRAME],
                     uint16_t lengths[ETH_MAC_TX_QUEUE],
                     uint8_t head,
                     uint8_t *tail,
                     uint8_t *out)
{
    if (*tail == head) {
        return 0;
    }
    uint16_t len = lengths[*tail];
    memcpy(out, queue[*tail], len);
    *tail = (uint8_t)((*tail + 1U) % ETH_MAC_TX_QUEUE);
    return (int)len;
}

bool eth_mac_send_frame(const uint8_t *data, uint16_t length)
{
    return queue_push(s_mac.tx_queue, s_mac.tx_length, &s_mac.tx_head, s_mac.tx_tail, data, length);
}

int eth_mac_receive_frame(uint8_t *data, uint16_t max_length)
{
    (void)max_length;
    return queue_pop(s_mac.rx_queue, s_mac.rx_length, s_mac.rx_head, &s_mac.rx_tail, data);
}

void eth_mac_poll(void)
{
    /* emulate Sync0 every control period */
    static uint32_t last_tick = 0;
    uint32_t current = timer_get_ticks();
    if (current != last_tick) {
        last_tick = current;
        s_mac.dc_time_ns += 1000000ULL; /* 1 kHz */
        if (s_mac.sync_cb != NULL) {
            s_mac.sync_cb(s_mac.sync_user);
        }
    }
}

uint64_t eth_mac_get_time_ns(void)
{
    return s_mac.dc_time_ns;
}

void eth_mac_adjust_time(int32_t ns_offset)
{
    s_mac.dc_time_ns += (int64_t)ns_offset;
}


void eth_mac_set_sync_callback(eth_sync_callback_t sync0_cb, void *user_data)
{
    s_mac.sync_cb = sync0_cb;
    s_mac.sync_user = user_data;
}
