add_library(utils
    q16.c
    q16.h
    vec3.c
    vec3.h
    matrix3.c
    matrix3.h
    filters.c
    filters.h
    lut_trig.c
    lut_trig.h
    log.c
    log.h
)

target_include_directories(utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
