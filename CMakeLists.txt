cmake_minimum_required(VERSION 3.20)
project(delta_cnc_controller LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(BUILD_DOCS "Enable documentation targets" ON)
option(ENABLE_G5 "Enable Bezier and NURBS G-code commands" ON)
option(ENABLE_OPCUA "Enable OPC UA server" ON)
set(TARGET_OS "host" CACHE STRING "Target operating system")
set_property(CACHE TARGET_OS PROPERTY STRINGS host qnx vxworks baget)

add_compile_options(-Wall -Wextra -Werror)

if(TARGET_OS STREQUAL "baget")
    add_definitions(-DBAGET_OS)
elseif(TARGET_OS STREQUAL "qnx")
    add_definitions(-DQNX_OS)
elseif(TARGET_OS STREQUAL "vxworks")
    add_definitions(-DVXWORKS_OS)
else()
    add_definitions(-DHOST_OS)
endif()

if(NOT DEFINED BAGET_ENDIAN)
    set(BAGET_ENDIAN "LE" CACHE STRING "Endianness for Baget")
endif()
set_property(CACHE BAGET_ENDIAN PROPERTY STRINGS LE BE)

if(BAGET_ENDIAN STREQUAL "LE")
    add_definitions(-DBAGET_ENDIAN_LE)
else()
    add_definitions(-DBAGET_ENDIAN_BE)
endif()

if(ENABLE_G5)
    add_definitions(-DENABLE_G5)
endif()

if(ENABLE_OPCUA)
    add_definitions(-DENABLE_OPCUA)
endif()

set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(SRC
    core/cnc_state.c
    core/command_processor.c
    core/main.c
    planner/planner.c
    planner/lookahead.c
    planner/trajectory_buffer.c
    planner/s_curve.c
    planner/splines/splines.c
    planner/splines/nurbs.c
    kinematics/delta.c
    motion/motion_control.c
    motion/sync.c
    ethcat/master.c
    ethcat/dc_sync.c
    ethcat/coemap.c
    cia402/cia402.c
    calib/calibration.c
    storage/storage.c
    storage/kv_store.c
    opcua/server.c
    gcode/parser.c
    gcode/console.c
    drivers/watchdog.c
    drivers/estop.c
    drivers/limits.c
    osal/osal.c
    utils/fixed.c
    utils/trig.c
    utils/matrix.c
    utils/filter.c
    utils/timer.c
    utils/log.c
    utils/crc16.c
    utils/assert.c
)

add_library(cnc_core STATIC ${SRC})
target_include_directories(cnc_core PUBLIC
    ${PROJECT_ROOT}
    core
    planner
    planner/splines
    kinematics
    motion
    ethcat
    cia402
    calib
    storage
    opcua
    gcode
    drivers
    osal
    utils
)

target_link_libraries(cnc_core PUBLIC m)

target_compile_definitions(cnc_core PUBLIC CONTROL_PERIOD_HZ=1000)

add_executable(cnc_firmware
    board/startup.c
    board/board.c
    board/network.c
    board/config.c
)

target_link_libraries(cnc_firmware PRIVATE cnc_core)

target_include_directories(cnc_firmware PRIVATE board)

if(TARGET_OS STREQUAL "host")
    add_executable(tests_host
        tests/test_runner.c
        tests/test_host.c
        tests/test_splines.c
        tests/test_kinematics.c
        tests/test_planner.c
        tests/test_storage.c
        tests/test_cia402.c
        tests/test_opcua.c
        tests/test_ethcat.c
        tests/test_console.c
    )
    target_link_libraries(tests_host PRIVATE cnc_core m)
    target_include_directories(tests_host PRIVATE tests)
endif()

if(BUILD_DOCS)
    find_package(Doxygen)
    find_package(Sphinx)
    if(DOXYGEN_FOUND)
        set(DOXYGEN_GENERATE_LATEX YES)
        set(DOXYGEN_EXTRACT_ALL YES)
        set(DOXYGEN_WARN_IF_UNDOCUMENTED YES)
        set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/docs/doxygen)
        set(DOXYGEN_GENERATE_HTML YES)
        set(DOXYGEN_GENERATE_XML YES)
        set(DOXYGEN_PROJECT_LOGO ${PROJECT_ROOT}/docs/images/logo.png)
        set(DOXYGEN_STRIP_FROM_PATH ${PROJECT_ROOT})
        set(DOXYGEN_WARN_LOGFILE ${CMAKE_CURRENT_BINARY_DIR}/docs/doxygen/warnings.log)
        doxygen_add_docs(docs_api ${PROJECT_ROOT}
            COMMENT "Generate API reference")
    endif()

    if(SPHINX_FOUND)
        set(SPHINX_SOURCE ${PROJECT_ROOT}/docs)
        set(SPHINX_BUILD ${CMAKE_CURRENT_BINARY_DIR}/docs/sphinx)
        add_custom_target(docs
            COMMAND ${CMAKE_COMMAND} -E env
                DOXYGEN_XML_DIR=${CMAKE_CURRENT_BINARY_DIR}/docs/doxygen/xml
                ${SPHINX_EXECUTABLE} -b html
                ${SPHINX_SOURCE}
                ${SPHINX_BUILD}/html
            COMMENT "Generate full documentation")
        if(TARGET docs_api)
            add_dependencies(docs docs_api)
        endif()
    endif()
endif()

install(TARGETS cnc_firmware RUNTIME DESTINATION bin)
