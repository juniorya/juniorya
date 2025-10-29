add_library(sim_support STATIC
    renderer.c
    visualizer3d.c
    configurator.c
)

target_include_directories(sim_support
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(sim_support
    PUBLIC
        utils
        planner
        kinematics
)

add_executable(delta_simulator
    simulator.c
)

target_link_libraries(delta_simulator
    PRIVATE
        sim_support
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
