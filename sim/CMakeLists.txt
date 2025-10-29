add_executable(delta_simulator
    simulator.c
    renderer.c
    renderer.h
)

target_include_directories(delta_simulator PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(delta_simulator
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
        drivers
        board
        opcua
)

set_target_properties(delta_simulator PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/simulator)
