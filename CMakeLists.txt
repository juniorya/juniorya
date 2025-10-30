cmake_minimum_required(VERSION 3.20)
project(delta_cnc C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_STANDARD_REQUIRED ON)

option(ENABLE_G5 "Enable G5/G5.2 spline commands" ON)
option(ENABLE_OPCUA "Enable OPC UA server" ON)
option(BUILD_DOCS "Build documentation" ON)

set(TARGET_OS "host" CACHE STRING "Target operating system")
set_property(CACHE TARGET_OS PROPERTY STRINGS host qnx vxworks baget qemu)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

include(cmake/os_detection.cmake)

add_subdirectory(utils)
add_subdirectory(osal)
add_subdirectory(core)
add_subdirectory(planner)
add_subdirectory(kinematics)
add_subdirectory(motion)
add_subdirectory(ethcat)
add_subdirectory(cia402)
add_subdirectory(calib)
add_subdirectory(storage)
add_subdirectory(gcode)
add_subdirectory(drivers)
add_subdirectory(board)
add_subdirectory(opcua)
add_subdirectory(ml)
add_subdirectory(sim)
add_subdirectory(ide)
add_subdirectory(vision)

set(PROJECT_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/osal
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/planner
    ${CMAKE_CURRENT_SOURCE_DIR}/kinematics
    ${CMAKE_CURRENT_SOURCE_DIR}/motion
    ${CMAKE_CURRENT_SOURCE_DIR}/ethcat
    ${CMAKE_CURRENT_SOURCE_DIR}/cia402
    ${CMAKE_CURRENT_SOURCE_DIR}/calib
    ${CMAKE_CURRENT_SOURCE_DIR}/storage
    ${CMAKE_CURRENT_SOURCE_DIR}/gcode
    ${CMAKE_CURRENT_SOURCE_DIR}/drivers
    ${CMAKE_CURRENT_SOURCE_DIR}/board
    ${CMAKE_CURRENT_SOURCE_DIR}/opcua
    ${CMAKE_CURRENT_SOURCE_DIR}/ml
    ${CMAKE_CURRENT_SOURCE_DIR}/ide
    ${CMAKE_CURRENT_SOURCE_DIR}/vision
)

add_library(delta_cnc_lib
    main.c
)

set_target_properties(delta_cnc_lib PROPERTIES OUTPUT_NAME cnc_firmware)

target_link_libraries(delta_cnc_lib
    PUBLIC
        utils
        osal
        core
        planner
        kinematics
        motion
        ethcat
        cia402
        calib
        storage
        gcode
        drivers
        board
        opcua
        ml
        vision
)

target_include_directories(delta_cnc_lib PUBLIC ${PROJECT_INCLUDE_DIRS})

if(TARGET_OS STREQUAL "host" OR TARGET_OS STREQUAL "qemu")
    enable_testing()
    add_subdirectory(tests)
endif()

add_subdirectory(docs)

if(TARGET_OS STREQUAL "qemu")
    add_custom_target(qemu_run
        COMMAND ${CMAKE_COMMAND} -E env QEMU_AUDIO_DRV=none
                qemu-system-mips64el -M malta -cpu mips64r2-generic -m 256M
                -kernel $<TARGET_FILE:delta_cnc_lib>
                -nographic -serial mon:stdio -monitor none
        DEPENDS delta_cnc_lib
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Booting firmware under QEMU MIPS64"
    )

    add_custom_target(qemu_smoke_test
        COMMAND ${CMAKE_COMMAND} -E env TARGET_OS=qemu PROJECT_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/tests/run_qemu_selftest.sh
        DEPENDS delta_cnc_lib
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Running QEMU MIPS64 integration self-test"
    )
endif()

