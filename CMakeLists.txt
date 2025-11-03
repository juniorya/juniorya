cmake_minimum_required(VERSION 3.21)
project(delta_cnc_portable LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

option(BUILD_DOCS "Build full documentation" ON)
option(BUILD_DOCS_API "Build API documentation" ON)
option(ENABLE_G5 "Enable optional G5/G5.2 spline commands" OFF)
option(ENABLE_OPCUA "Enable OPC UA server" ON)
option(ENABLE_DDS "Enable DDS middleware" ON)

set(TARGET_OS "pc-linux" CACHE STRING "Target operating system")
set_property(CACHE TARGET_OS PROPERTY STRINGS pc-linux qemu-mips64 baget-komdiv64)

set(SYNC0_RATE_HZ 8000 CACHE STRING "EtherCAT Sync0 frequency")

add_compile_options(-Wall -Wextra -Werror)

if(ENABLE_G5)
    add_compile_definitions(ENABLE_G5=1)
else()
    add_compile_definitions(ENABLE_G5=0)
endif()

if(ENABLE_OPCUA)
    add_compile_definitions(ENABLE_OPCUA=1)
else()
    add_compile_definitions(ENABLE_OPCUA=0)
endif()

if(ENABLE_DDS)
    add_compile_definitions(ENABLE_DDS=1)
else()
    add_compile_definitions(ENABLE_DDS=0)
endif()

add_compile_definitions(SYNC0_RATE_HZ=${SYNC0_RATE_HZ})

if(TARGET_OS STREQUAL "pc-linux")
    add_compile_definitions(TARGET_OS_PC_LINUX=1)
elseif(TARGET_OS STREQUAL "qemu-mips64")
    add_compile_definitions(TARGET_OS_QEMU_MIPS64=1)
elseif(TARGET_OS STREQUAL "baget-komdiv64")
    add_compile_definitions(TARGET_OS_BAGET=1)
else()
    message(FATAL_ERROR "Unsupported TARGET_OS ${TARGET_OS}")
endif()

set(CORE_SOURCES
    board/board.c
    calib/calib.c
    cia402/cia402.c
    core/cnc.c
    drivers/drivers.c
    ethcat/ethcat.c
    gcode/gcode.c
    kinematics/delta.c
    motion/motion.c
    opcua/opcua.c
    osal/osal.c
    planner/planner.c
    planner/lookahead/lookahead.c
    planner/s_curve/s_curve.c
    planner/splines/splines.c
    storage/storage.c
    utils/q16.c
    utils/vec3.c
    utils/log.c
    dds/dds.c
)

add_library(cnc_core STATIC ${CORE_SOURCES})

target_include_directories(cnc_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/calib
        ${CMAKE_CURRENT_SOURCE_DIR}/cia402
        ${CMAKE_CURRENT_SOURCE_DIR}/core
        ${CMAKE_CURRENT_SOURCE_DIR}/drivers
        ${CMAKE_CURRENT_SOURCE_DIR}/ethcat
        ${CMAKE_CURRENT_SOURCE_DIR}/gcode
        ${CMAKE_CURRENT_SOURCE_DIR}/kinematics
        ${CMAKE_CURRENT_SOURCE_DIR}/motion
        ${CMAKE_CURRENT_SOURCE_DIR}/opcua
        ${CMAKE_CURRENT_SOURCE_DIR}/osal
        ${CMAKE_CURRENT_SOURCE_DIR}/planner
        ${CMAKE_CURRENT_SOURCE_DIR}/planner/lookahead
        ${CMAKE_CURRENT_SOURCE_DIR}/planner/s_curve
        ${CMAKE_CURRENT_SOURCE_DIR}/planner/splines
        ${CMAKE_CURRENT_SOURCE_DIR}/storage
        ${CMAKE_CURRENT_SOURCE_DIR}/utils
        ${CMAKE_CURRENT_SOURCE_DIR}/dds)

target_link_libraries(cnc_core PUBLIC m)

add_executable(cnc_firmware main.c)
target_link_libraries(cnc_firmware PRIVATE cnc_core)

add_executable(opcua_dds_bridge tools/opcua_dds_bridge.c)
target_link_libraries(opcua_dds_bridge PRIVATE cnc_core)

add_executable(tests_host
    tests/unit/test_main.c
    tests/unit/test_planner.c
    tests/unit/test_kinematics.c
    tests/unit/test_storage.c
    tests/integration/test_selftest.c)

target_link_libraries(tests_host PRIVATE cnc_core)

include(CTest)
add_test(NAME unit_tests COMMAND tests_host)

find_package(Doxygen)
if(DOXYGEN_FOUND AND BUILD_DOCS_API)
    set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/docs/api)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    add_custom_target(docs_api
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating Doxygen API documentation")
endif()

find_package(Sphinx)
if(SPHINX_FOUND AND BUILD_DOCS)
    add_custom_target(docs
        COMMAND ${SPHINX_EXECUTABLE} -b html ${CMAKE_CURRENT_SOURCE_DIR}/docs/source ${CMAKE_CURRENT_BINARY_DIR}/docs/html
        COMMAND ${SPHINX_EXECUTABLE} -b latex ${CMAKE_CURRENT_SOURCE_DIR}/docs/source ${CMAKE_CURRENT_BINARY_DIR}/docs/latex
        COMMENT "Building Sphinx documentation")
endif()
