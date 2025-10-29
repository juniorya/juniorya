#ifndef TESTS_TEST_SUITE_H
#define TESTS_TEST_SUITE_H

/**
 * @file test_suite.h
 * @brief Declarations for host-side unit and integration test entry points.
 */

/**
 * @brief Execute integration smoke test of the host control loop entry point.
 */
void test_host(void);

/**
 * @brief Execute spline subsystem validation checks.
 */
void test_splines(void);

/**
 * @brief Execute delta kinematics forward/inverse validation checks.
 */
void test_kinematics(void);

/**
 * @brief Execute planner look-ahead and segment generation validation checks.
 */
void test_planner(void);

/**
 * @brief Execute persistent storage subsystem checks.
 */
void test_storage(void);

/**
 * @brief Execute CiA-402 state machine sanity checks.
 */
void test_cia402(void);

/**
 * @brief Execute OPC UA server smoke tests.
 */
void test_opcua(void);

/**
 * @brief Execute EtherCAT master abstraction smoke tests.
 */
void test_ethcat(void);

/**
 * @brief Execute console and G-code parser validation checks.
 */
void test_console(void);

#endif /* TESTS_TEST_SUITE_H */
