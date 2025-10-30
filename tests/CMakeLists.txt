add_executable(tests_host
    test_suite.c
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/data/demo_project.cncp
               ${CMAKE_CURRENT_BINARY_DIR}/demo_project.cncp COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/data/keba_drive.xml
               ${CMAKE_CURRENT_BINARY_DIR}/keba_drive.xml COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/data/demo_program.st
               ${CMAKE_CURRENT_BINARY_DIR}/demo_program.st COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/data/tinyml_profile.ini
               ${CMAKE_CURRENT_BINARY_DIR}/tinyml_profile.ini COPYONLY)

target_link_libraries(tests_host
    PRIVATE
        utils
        planner
        kinematics
        motion
        ethcat
        cia402
        calib
        storage
        gcode
        board
        ml
        sim_support
        ide_support
)

target_compile_definitions(tests_host PRIVATE TEST_DATA_DIR="${CMAKE_CURRENT_BINARY_DIR}")

add_test(NAME unit_tests COMMAND tests_host)
