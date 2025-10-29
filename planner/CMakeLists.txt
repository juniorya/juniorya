add_library(planner
    planner.h
    planner.c
    lookahead/lookahead.h
    lookahead/lookahead.c
    s_curve/s_curve.h
    s_curve/s_curve.c
    splines/splines.h
    splines/splines.c
)

target_include_directories(planner PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/..)

target_link_libraries(planner PUBLIC utils ml m)
