add_library(drivers
    drivers.c
    drivers.h
)

target_include_directories(drivers PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(drivers PUBLIC utils)
