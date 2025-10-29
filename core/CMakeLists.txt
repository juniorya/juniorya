add_library(core
    cnc.c
    cnc.h
    commands.c
    commands.h
    state_machine.c
    state_machine.h
)

target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(core PUBLIC utils osal planner motion ethcat cia402 storage gcode drivers)
