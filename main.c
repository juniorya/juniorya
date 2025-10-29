/**
 * @file main.c
 * @brief Entry point for the CNC firmware.
 */
#include "core/cnc.h"
#include "planner/planner.h"
#include "motion/motion.h"
#include "ethcat/ethcat.h"
#include "drivers/drivers.h"
#include "board/board.h"

int main(void)
{
    drivers_init();
    cnc_context_t cnc;
    cnc_init(&cnc);
    planner_limits_t limits = {q16_from_int(200), q16_from_int(1000), q16_from_int(5000)};
    planner_t planner;
    planner_init(&planner, &limits);
    ethcat_master_t master;
    ethcat_master_init(&master, 1000000U);
    board_configure_network(&master);
    motion_controller_t motion;
    motion_init(&motion, &planner, &master);
    vec3_q16 start = {0, 0, 0};
    vec3_q16 end = {q16_from_int(10), 0, 0};
    planner_enqueue_linear(&planner, &start, &end);
    motion_sync0_tick(&motion, q16_from_int(1));
    return 0;
}
