#include "board/config.h"
#include "board/board.h"
#include "core/cnc_state.h"
#include "core/command_processor.h"
#include "cia402/cia402.h"
#include "ethcat/master.h"
#include "motion/motion_control.h"
#include "gcode/parser.h"
#include "utils/timer.h"
#include "drivers/eth_mac.h"

static ethcat_master_t g_master;
static planner_queue_t g_planner;
static cia402_axis_t g_axes[ECAT_MAX_SLAVES];
static gcode_parser_t g_parser;
static command_queue_t g_cmd_queue;
static motion_controller_t g_motion;
static cnc_runtime_t g_runtime;

static void sync0_callback(void *user)
{
    (void)user;
    ethcat_master_sync0_handler(&g_master);
    motion_controller_tick(&g_motion);
}

int main(void)
{
    board_clock_init();
    board_gpio_init();
    board_console_init();
    board_emac_init();
    board_load_configuration();

    delta_init(&g_board_config.delta);
    planner_init(&g_planner, CONTROL_PERIOD_US);
    gcode_parser_init(&g_parser);
    command_queue_init(&g_cmd_queue);
    cnc_runtime_init(&g_runtime);

    ethcat_master_init(&g_master, &g_board_config);
    ethcat_master_scan(&g_master);
    ethcat_master_configure(&g_master);
    eth_mac_set_sync_callback(sync0_callback, NULL);

    for (int axis = 0; axis < ECAT_MAX_SLAVES; ++axis) {
        cia402_axis_init(&g_axes[axis], CIA402_MODE_CSP);
    }
    motion_controller_init(&g_motion, &g_planner, &g_master, g_axes);

    while (1) {
        timer_tick_isr();
        ethcat_master_process(&g_master);
        command_processor_step(&g_cmd_queue, &g_runtime, &g_parser, &g_planner, g_axes);
    }
}
