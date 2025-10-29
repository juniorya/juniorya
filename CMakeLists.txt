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

set(SOURCES
    core/cnc.c
    planner/planner.c
    planner/splines/splines.c
    kinematics/delta_kinematics.c
    motion/motion.c
    ethcat/ethcat.c
    cia402/cia402.c
    calib/calib.c
    storage/storage.c
    opcua/opcua.c
    gcode/gcode.c
    drivers/drivers.c
    board/board.c
    osal/osal_host.c
    utils/vec3.c
    utils/crc16.c
    main.c)

add_library(cnc_core STATIC
    core/cnc.c
    planner/planner.c
    planner/splines/splines.c
    kinematics/delta_kinematics.c
    motion/motion.c
    ethcat/ethcat.c
    cia402/cia402.c
    calib/calib.c
    storage/storage.c
    opcua/opcua.c
    gcode/gcode.c
    drivers/drivers.c
    board/board.c
    osal/osal_host.c
    utils/vec3.c
    utils/crc16.c)

target_include_directories(cnc_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
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
    board
    osal
    utils)

target_link_libraries(cnc_core PUBLIC m)

target_compile_definitions(cnc_core PUBLIC CONTROL_PERIOD_HZ=1000)

add_executable(cnc_firmware main.c)
target_link_libraries(cnc_firmware PRIVATE cnc_core)

target_include_directories(cnc_firmware PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

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
        tests/test_console.c)
    target_link_libraries(tests_host PRIVATE cnc_core m)
    target_include_directories(tests_host PRIVATE tests)
endif()

if(BUILD_DOCS)
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        set(DOXYGEN_GENERATE_LATEX YES)
        set(DOXYGEN_EXTRACT_ALL YES)
        set(DOXYGEN_WARN_IF_UNDOCUMENTED YES)
        set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/docs/doxygen)
        set(DOXYGEN_GENERATE_HTML YES)
        set(DOXYGEN_GENERATE_XML YES)
        set(DOXYGEN_STRIP_FROM_PATH ${CMAKE_CURRENT_SOURCE_DIR})
        doxygen_add_docs(docs_api ${CMAKE_CURRENT_SOURCE_DIR}
            COMMENT "Generate API reference")
    endif()
    find_package(Sphinx)
    if(SPHINX_FOUND)
        set(SPHINX_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/docs)
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
