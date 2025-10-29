add_executable(tests_host
    test_suite.c
)

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
)

add_test(NAME unit_tests COMMAND tests_host)
