#include "test_suite.h"
#include <stdio.h>

/**
 * @brief Entry point that executes all host-side unit and integration tests.
 */
int main(void)
{
    puts("[tests] Starting host test suite...");
    test_host();
    test_splines();
    test_kinematics();
    test_planner();
    test_storage();
    test_cia402();
    test_opcua();
    test_ethcat();
    test_console();
    puts("[tests] All host tests completed successfully.");
    return 0;
}
