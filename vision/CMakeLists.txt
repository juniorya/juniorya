add_library(vision
    vision.c
    vision.h
)

target_include_directories(vision PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/..)

target_link_libraries(vision PUBLIC utils osal)
