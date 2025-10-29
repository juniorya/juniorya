add_library(gcode
    gcode.c
    gcode.h
)

target_include_directories(gcode PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(gcode PUBLIC utils planner)
