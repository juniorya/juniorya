add_library(calib
    calib.c
    calib.h
)

target_include_directories(calib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(calib PUBLIC utils storage)
