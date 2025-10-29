cmake_minimum_required(VERSION 3.16)
project(delta_controller C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Werror)

set(COMMON_SOURCES
    utils/fixed.c
    utils/trig.c
    utils/matrix.c
    utils/crc16.c
    utils/timer.c
    utils/filter.c
    kinematics/delta.c
    planner/planner.c
    motion/motion_control.c
    ethcat/master.c
    cia402/cia402.c
    gcode/parser.c
    core/cnc_state.c
    core/command_processor.c
    drivers/eth_mac.c
    drivers/gpio.c
    drivers/uart.c
    board/board.c
)

add_executable(firmware_stm32
    core/main.c
    ${COMMON_SOURCES}
)

target_include_directories(firmware_stm32 PRIVATE
    .
    board
    utils
    kinematics
    planner
    motion
    ethcat
    cia402
    gcode
    core
    drivers
)

target_link_libraries(firmware_stm32 m)

option(HOST_TESTS "Build host tests" OFF)
if (HOST_TESTS)
    add_executable(tests_host
        tests/test_host.c
        ${COMMON_SOURCES}
    )
    target_include_directories(tests_host PRIVATE
        .
        board
        utils
        kinematics
        planner
        motion
        ethcat
        cia402
        gcode
        core
        drivers
    )
    target_link_libraries(tests_host m)
endif()
