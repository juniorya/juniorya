add_executable(tests_host
    test_suite.c
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/data/demo_project.cncp
               ${CMAKE_CURRENT_BINARY_DIR}/demo_project.cncp COPYONLY)

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
        board
        ml
        sim_support
        ide_support
)

add_test(NAME unit_tests COMMAND tests_host)
