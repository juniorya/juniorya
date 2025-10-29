#include "board.h"
#include "drivers/gpio.h"
#include "drivers/eth_mac.h"
#include "drivers/uart.h"
#include <stddef.h>
#include "utils/fixed.h"

board_runtime_config_t g_board_config;

void board_clock_init(void)
{
    /* clock tree configured in CubeMX normally; placeholder for deterministic startup */
}

void board_gpio_init(void)
{
    gpio_init();
}

void board_emac_init(void)
{
    eth_mac_config_t cfg = {
        .mac_address = {0x02, 0x12, 0x34, 0x56, 0x78, 0x9A},
        .phy_address = 0U,
    };
    eth_mac_init(&cfg, NULL, NULL);
}

void board_console_init(void)
{
    uart_init(115200U);
}

void board_load_configuration(void)
{
    g_board_config.delta.R_base = q16_16_from_float(0.300f);
    g_board_config.delta.r_eff = q16_16_from_float(0.100f);
    g_board_config.delta.L_upper = q16_16_from_float(0.300f);
    g_board_config.delta.L_lower = q16_16_from_float(0.400f);
    g_board_config.delta.z_offset = q16_16_from_float(0.200f);
    g_board_config.delta.soft_xyz_min[0] = q16_16_from_float(-0.200f);
    g_board_config.delta.soft_xyz_min[1] = q16_16_from_float(-0.200f);
    g_board_config.delta.soft_xyz_min[2] = q16_16_from_float(-0.500f);
    g_board_config.delta.soft_xyz_max[0] = q16_16_from_float(0.200f);
    g_board_config.delta.soft_xyz_max[1] = q16_16_from_float(0.200f);
    g_board_config.delta.soft_xyz_max[2] = q16_16_from_float(-0.100f);

    g_board_config.axis_velocity_limit = q16_16_from_float(0.150f);
    g_board_config.axis_acceleration_limit = q16_16_from_float(1.000f);
    g_board_config.axis_jerk_limit = q16_16_from_float(5.000f);
    g_board_config.default_mode_of_operation = 8U; /* CSP */

    for (int i = 0; i < ECAT_MAX_SLAVES; ++i) {
        g_board_config.slaves[i].vendor_id = 0x000000abU;
        g_board_config.slaves[i].product_code = 0x00001000U + (uint32_t)i;
        g_board_config.slaves[i].alias = (uint16_t)(i + 1);
    }
}
